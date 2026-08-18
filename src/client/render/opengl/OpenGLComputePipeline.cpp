#include "OpenGLComputePipeline.h"

#include <vector>

#include "glad/glad.h"
#include "spirv_glsl.hpp"

#include "common/util/FileHelper.h"

namespace voxel_game::client::render::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const std::string& computeShaderPath) {
		std::vector<uint32_t> spirv = util::readFile<uint32_t>(computeShaderPath);
		spirv_cross::CompilerGLSL compiler(std::move(spirv));
		spirv_cross::CompilerGLSL::Options options;
		options.version = 430;
		compiler.set_common_options(options);
		const std::string glsl = compiler.compile();
		const char* glslCStr = glsl.c_str();

		const unsigned int computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &glslCStr, nullptr);
		glCompileShader(computeShader);

		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, computeShader);
		glLinkProgram(mShaderProgram);
	}

	OpenGLComputePipeline::~OpenGLComputePipeline() {
		glDeleteProgram(mShaderProgram);
	}
}
