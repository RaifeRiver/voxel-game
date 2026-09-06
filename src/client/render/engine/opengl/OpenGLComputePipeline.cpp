#include "OpenGLComputePipeline.h"

#include "glad/glad.h"
#include "tracy/TracyOpenGL.hpp"

#include "OpenGLDescriptorSet.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const Shader& computeShaderGLSL) {
		const std::vector<uint32_t>& computeShaderData = computeShaderGLSL.getSPIRV();

		const std::string computeShaderCode = opengl_util::convertShader(computeShaderData);
		const char* computeShaderCodeChars = computeShaderCode.c_str();

		const unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeShaderCodeChars, nullptr);
		glCompileShader(computeShader);

		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, computeShader);
		glLinkProgram(mShaderProgram);

		glDeleteShader(computeShader);

		mPushConstants = opengl_util::getPushConstants(1, &computeShaderData, mShaderProgram);
	}

	void OpenGLComputePipeline::bind() {
		glUseProgram(mShaderProgram);
	}

	void OpenGLComputePipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		dynamic_cast<OpenGLDescriptorSet*>(descriptorSet)->bind(set);
	}

	void OpenGLComputePipeline::setPushConstants(void* pushConstants) {
		opengl_util::setPushConstantData(mPushConstants, pushConstants);
	}

	OpenGLComputePipeline::~OpenGLComputePipeline() {
		glDeleteProgram(mShaderProgram);
	}

	void OpenGLComputePipeline::dispatch_(const uint32_t x, const uint32_t y, const uint32_t z, const std::string& label) {
		TracyGpuZoneTransient(tracyZone, label.c_str(), true);
		glDispatchCompute(x, y, z);
	}
}
