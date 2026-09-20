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
#include "client/render/engine/RenderPipeline.h"

namespace voxel_game::client::render::engine::opengl {
	unsigned int toOpenGLPrimitiveTopology(PrimitiveTopology topology);

	unsigned int toOpenGLPolygonMode(PolygonMode mode);

	unsigned int toOpenGLCullFace(CullMode mode);

	unsigned int toOpenGLFrontFace(FrontFace face);

	class OpenGLRenderPipeline : public RenderPipeline {
	public:
		explicit OpenGLRenderPipeline(const RenderPipelineBuilder* builder);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		void setPushConstants(void* pushConstants) override;

		void bindIndexBuffer(GPUBuffer* buffer) override;

		~OpenGLRenderPipeline() override;

	protected:
		void draw_(uint32_t vertexCount, uint32_t firstVertex, const std::string& label) override;

		void drawIndexed_(uint32_t indexCount, uint32_t firstIndex, const std::string& label) override;

		void drawIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) override;

		void drawIndexedIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) override;

	private:
		unsigned int mShaderProgram;
		unsigned int mVertexArray = 0;
		std::vector<opengl_util::PushConstant> mPushConstants;

		unsigned int mPrimitiveTopology = 0;
		unsigned int mPolygonMode = 0;
		float mLineWidth = 1.0f;
		unsigned int mCullFace = 0;
		unsigned int mFrontFace = 0;
		BlendMode mBlendMode = BlendMode::NONE;
	};

	class OpenGLRenderPipelineBuilder : public RenderPipelineBuilder {
	public:
		OpenGLRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader);

		std::unique_ptr<RenderPipeline> build() override;
	};
}
