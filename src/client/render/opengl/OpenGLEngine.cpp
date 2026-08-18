#include "OpenGLEngine.h"

#include <stdexcept>

#include "glad/glad.h"

#include "OpenGLComputePipeline.h"

namespace voxel_game::client::render::opengl {
	OpenGLEngine::OpenGLEngine(ecs::ECSRegistry& registry) {
		auto& window = registry.getResource<window::Window>();

		initOpenGL(window);

		registry.getSystemManager().registerSystem(ecs::Stage::PRE_RENDER, [this](ecs::ECSRegistry&, float) {
			preRender();
		});
		registry.getSystemManager().registerSystem(ecs::Stage::POST_RENDER, [this](ecs::ECSRegistry& r, float) {
			postRender(r.getResource<window::Window>());
		});
	}

	std::unique_ptr<GPUImage> OpenGLEngine::allocateImage(glm::ivec3, ImageFormat, ImageUsage, ImageType) {
		throw std::runtime_error("Not implemented");
	}

	std::unique_ptr<ComputePipeline> OpenGLEngine::createComputePipeline(const std::string& computeShader) {
		return std::make_unique<OpenGLComputePipeline>(computeShader);
	}

	std::unique_ptr<DescriptorAllocatorBuilder> OpenGLEngine::createDescriptorAllocatorBuilder() {
		throw std::runtime_error("Not implemented");
	}

	void OpenGLEngine::waitForGPU() {
		throw std::runtime_error("Not implemented");
	}

	void OpenGLEngine::initOpenGL(window::Window& window) {
		if (gladLoadGL() == 0) {
			throw std::runtime_error("Failed to initialise GLAD");
		}

		const glm::uvec2 windowSize = window.getSize();

		glViewport(0, 0, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));
		window.setResizeCallback([](const glm::uvec2 size) {
			glViewport(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
		});

		mRenderImage = std::make_unique<OpenGLRenderImage>(glm::uvec3{windowSize, 1});
	}

	void OpenGLEngine::preRender() {

	}

	void OpenGLEngine::postRender(window::Window& window) {
		window.swapOpenGLBuffers();
	}
}
