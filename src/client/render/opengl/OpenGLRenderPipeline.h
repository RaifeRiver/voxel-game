#pragma once

#include "client/render/RenderPipeline.h"

namespace voxel_game::client::render::opengl {
	class OpenGLRenderPipeline : public RenderPipeline {
	public:
		explicit OpenGLRenderPipeline(const RenderPipelineBuilder* builder);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		~OpenGLRenderPipeline() override;

	protected:
		void draw_(uint32_t vertexCount, uint32_t firstVertex) override;

	private:
		unsigned int mShaderProgram;
		unsigned int mVertexArray = 0;
	};

	class OpenGLRenderPipelineBuilder : public RenderPipelineBuilder {
	public:
		OpenGLRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader);

		std::unique_ptr<RenderPipeline> build() override;
	};
}
