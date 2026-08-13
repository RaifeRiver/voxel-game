#pragma once

#include "OpenGLRenderImage.h"
#include "client/render/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::opengl {
	class OpenGLEngine : public RenderEngine {
	public:
		explicit OpenGLEngine(ecs::ECSRegistry& registry);

		GPUImage* allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) override;

		GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

	private:
		std::unique_ptr<OpenGLRenderImage> mRenderImage;

		void initOpenGL(window::Window& window);

		void preRender();

		void postRender(window::Window& window);
	};
}
