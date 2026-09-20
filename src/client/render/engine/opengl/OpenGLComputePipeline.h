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

#pragma once

#include "OpenGLUtil.h"
#include "client/render/engine/ComputePipeline.h"
#include "client/render/engine/Shader.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLComputePipeline : public ComputePipeline {
	public:
		explicit OpenGLComputePipeline(const ComputePipelineBuilder* builder);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		void setPushConstants(void* pushConstants) override;

		~OpenGLComputePipeline() override;

	protected:
		void dispatch_(uint32_t x, uint32_t y, uint32_t z, const std::string& label) override;

	private:
		unsigned int mShaderProgram;
		std::vector<opengl_util::PushConstant> mPushConstants;
	};

	class OpenGLComputePipelineBuilder : public ComputePipelineBuilder {
	public:
		explicit OpenGLComputePipelineBuilder(const Shader& computeShader);

		std::unique_ptr<ComputePipeline> build() override;
	};
}
