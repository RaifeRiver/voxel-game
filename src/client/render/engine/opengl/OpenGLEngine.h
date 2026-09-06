#pragma once

#include "glad/glad.h"

#include "OpenGLImage.h"
#include "client/render/engine/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLEngine : public RenderEngine {
	public:
		explicit OpenGLEngine(ecs::ECSRegistry& registry);

		std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) override;

		std::unique_ptr<ComputePipeline> createComputePipeline(const Shader& computeShader) override;

		std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader) override;

		std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() override;

		[[nodiscard]] GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

		[[nodiscard]] GPUImage& getDepthImage() override {
			return *mDepthImage;
		}

		void beginRendering() override;

		void endRendering() override;

		void waitForGPU() override;

		void destroy() override;

	protected:
		std::unique_ptr<GPUBuffer> allocateBuffer_(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType) override;

	private:
		std::unique_ptr<OpenGLImage> mRenderImage = nullptr;
		std::unique_ptr<OpenGLImage> mDepthImage = nullptr;
		GLuint mFramebufferObject = 0;
		GLsync mRenderFences[FRAME_OVERLAP] = {};

		void initOpenGL(window::Window& window);

		void preRender();

		void postRender(window::Window& window);
	};
}
