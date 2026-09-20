/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "OpenGLComputePipeline.h"

#include "glad/glad.h"
#include "tracy/TracyOpenGL.hpp"

#include "OpenGLDescriptorSet.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLComputePipeline::OpenGLComputePipeline(const ComputePipelineBuilder* builder) {
		const std::vector<uint32_t>& computeShaderData = builder->getComputeShader().getSPIRV();

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

	OpenGLComputePipelineBuilder::OpenGLComputePipelineBuilder(const Shader& computeShader) : ComputePipelineBuilder(computeShader) {}

	std::unique_ptr<ComputePipeline> OpenGLComputePipelineBuilder::build() {
		return std::make_unique<OpenGLComputePipeline>(this);
	}
}
