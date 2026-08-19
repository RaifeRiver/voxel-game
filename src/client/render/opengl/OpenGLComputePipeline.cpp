#include "OpenGLComputePipeline.h"

#include <vector>

#include "OpenGLDescriptorSet.h"
#include "glad/glad.h"
#include "spirv_glsl.hpp"

#include "common/util/FileHelper.h"

namespace voxel_game::client::render::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const std::string& computeShaderPath) {
		std::vector<uint32_t> spirv = util::readFile<uint32_t>(computeShaderPath);
		spirv_cross::CompilerGLSL compiler(std::move(spirv));
		const spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();
		spirv_cross::CompilerGLSL::Options options;
		options.version = 430;
		compiler.set_common_options(options);

		auto flattenBindings = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources) {
			for (const spirv_cross::Resource& resource : resources) {
				const uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
				compiler.set_decoration(resource.id, spv::DecorationBinding, (set << 4) | binding);
			}
		};
		flattenBindings(shaderResources.sampled_images);
		flattenBindings(shaderResources.separate_images);
		flattenBindings(shaderResources.storage_images);
		flattenBindings(shaderResources.separate_samplers);
		flattenBindings(shaderResources.uniform_buffers);
		flattenBindings(shaderResources.storage_buffers);

		const std::string glsl = compiler.compile();
		const char* glslCStr = glsl.c_str();

		const unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &glslCStr, nullptr);
		glCompileShader(computeShader);

		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, computeShader);
		glLinkProgram(mShaderProgram);

		glDeleteShader(computeShader);
	}

	void OpenGLComputePipeline::bind() {
		glUseProgram(mShaderProgram);
	}

	void OpenGLComputePipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		dynamic_cast<OpenGLDescriptorSet*>(descriptorSet)->bind(set);
	}

	OpenGLComputePipeline::~OpenGLComputePipeline() {
		glDeleteProgram(mShaderProgram);
	}

	void OpenGLComputePipeline::dispatch_(const uint32_t x, const uint32_t y, const uint32_t z) {
		glDispatchCompute(x, y, z);
	}
}
