#pragma once

#include "OpenGLImage.h"
#include "client/render/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::opengl {
	class OpenGLEngine : public RenderEngine {
	public:
		explicit OpenGLEngine(ecs::ECSRegistry& registry);

		std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) override;

		std::unique_ptr<ComputePipeline> createComputePipeline(const std::string& computeShader) override;

		std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) override;

		std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() override;

		GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

		void beginRendering() override;

		void endRendering() override;

		void waitForGPU() override;

		void destroy() override;

	private:
		std::unique_ptr<OpenGLImage> mRenderImage = nullptr;
		GLuint mFramebufferObject = 0;

		void initOpenGL(window::Window& window);

		void preRender() const;

		void postRender(window::Window& window) const;
	};
}
