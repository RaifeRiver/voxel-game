#include "OpenGLComputePipeline.h"

#include "OpenGLDescriptorSet.h"
#include "OpenGLUtil.h"
#include "glad/glad.h"

namespace voxel_game::client::render::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const std::string& computeShaderPath) {
		const std::string computeShaderCode = opengl_util::loadShaderCode(computeShaderPath);
		const char* computeShaderCodeChars = computeShaderCode.c_str();

		const unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeShaderCodeChars, nullptr);
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
