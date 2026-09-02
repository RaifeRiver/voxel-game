#pragma once

#include <string>
#include <vector>

#include "shaderc/shaderc.hpp"

#include "common/resource/ResourceManager.h"

namespace voxel_game::client::render::engine {
	enum class ShaderStage : uint32_t {
		NONE = 0,
		VERTEX = 1 << 0,
		FRAGMENT = 1 << 1,
		COMPUTE = 1 << 2
	};

	inline ShaderStage operator|(ShaderStage a, ShaderStage b) {
		return static_cast<ShaderStage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline uint32_t operator&(ShaderStage a, ShaderStage b) {
		return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
	}

	shaderc_shader_kind toShaderCStage(ShaderStage stage);

	struct ShaderIncludeUserData {
		std::string contents;
		std::string path;
	};

	class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface {
	public:
		explicit ShaderIncluder(resource::ResourceManager& resourceManager);

		shaderc_include_result* GetInclude(const char* requestedSource, shaderc_include_type type, const char* requestingSource, size_t includeDepth) override;

		void ReleaseInclude(shaderc_include_result* data) override;

	private:
		resource::ResourceManager& mResourceManager;
	};

	void initShaderCompiler(resource::ResourceManager& resourceManager);

	std::vector<uint32_t> compileGLSL(const std::string& glsl, ShaderStage stage);
}
