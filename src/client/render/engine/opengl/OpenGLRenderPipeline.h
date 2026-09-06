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
		void draw_(uint32_t vertexCount, uint32_t firstVertex) override;

		void drawIndexed_(uint32_t indexCount, uint32_t firstIndex) override;

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
