#include "OpenGLComputePipeline.h"

#include <stdexcept>

#include "glad/glad.h"

#include "OpenGLDescriptorSet.h"
#include "OpenGLUtil.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const std::string& computeShaderPath) {
		const std::vector<uint32_t> computeShaderData = util::readFile<uint32_t>(computeShaderPath);

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

	void OpenGLComputePipeline::dispatch_(const uint32_t x, const uint32_t y, const uint32_t z) {
		glDispatchCompute(x, y, z);
	}
}
