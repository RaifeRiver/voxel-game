#include "Shader.h"

#include "common/util/FileHelper.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::engine {
	shaderc_shader_kind toShaderCStage(const ShaderStage stage) {
		switch (stage) {
			case ShaderStage::VERTEX:
				return shaderc_vertex_shader;
			case ShaderStage::FRAGMENT:
				return shaderc_fragment_shader;
			case ShaderStage::COMPUTE:
				return shaderc_compute_shader;
			default:
				throw std::runtime_error("Unsupported shader stage");
		}
	}

	ShaderIncluder::ShaderIncluder(resource::ResourceManager& resourceManager) : mResourceManager(resourceManager) {}

	shaderc_include_result* ShaderIncluder::GetInclude(const char* requestedSource, shaderc_include_type, const char*, size_t) {
		auto name = std::string(requestedSource);
		name.insert(name.find(':') + 1, "shaders/");
		auto [found, path] = mResourceManager.findResource(name, ".glsl", resource::ResourceType::ASSET);
		auto* userData = new ShaderIncludeUserData();
		if (found) {
			userData->contents = util::readFileAsString(path);
			userData->path = path;
		}
		else {
			LOG_ERROR("Shader include missing: {}", requestedSource);
			userData->contents = "Error finding shader include";
			userData->path = "";
		}
		auto* result = new shaderc_include_result();
		result->content = userData->contents.c_str();
		result->content_length = userData->contents.length();
		result->source_name = userData->path.c_str();
		result->source_name_length = userData->path.length();
		result->user_data = userData;
		return result;
	}

	void ShaderIncluder::ReleaseInclude(shaderc_include_result* data) {
		if (data) {
			delete static_cast<ShaderIncludeUserData*>(data->user_data);
			delete data;
		}
	}

	static const shaderc::Compiler SHADERC_COMPILER;
	static shaderc::CompileOptions SHADERC_OPTIONS;

	void initShaderCompiler(resource::ResourceManager& resourceManager) {
		SHADERC_OPTIONS.SetOptimizationLevel(shaderc_optimization_level_performance);
		SHADERC_OPTIONS.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
		SHADERC_OPTIONS.SetIncluder(std::make_unique<ShaderIncluder>(resourceManager));
	}

	std::vector<uint32_t> compileGLSL(const std::string& glsl, const ShaderStage stage) {
		const shaderc::SpvCompilationResult result = SHADERC_COMPILER.CompileGlslToSpv(glsl, toShaderCStage(stage), "string", SHADERC_OPTIONS);
		if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
			throw std::runtime_error("Error compiling shader: " + result.GetErrorMessage());
		}
		return {result.cbegin(), result.cend()};
	}
}
