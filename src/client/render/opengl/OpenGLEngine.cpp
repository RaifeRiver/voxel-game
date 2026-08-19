#include "OpenGLEngine.h"

#include <iostream>
#include <stdexcept>

#include "glad/glad.h"

#include "OpenGLComputePipeline.h"
#include "OpenGLDescriptorAllocator.h"
#include "OpenGLImage.h"

namespace voxel_game::client::render::opengl {
	OpenGLEngine::OpenGLEngine(ecs::ECSRegistry& registry) {
		auto& window = registry.getResource<window::Window>();

		initOpenGL(window);

		registry.getSystemManager().registerSystem(ecs::Stage::PRE_RENDER, [this](ecs::ECSRegistry& r, float) {
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
		return std::make_unique<OpenGLDescriptorAllocatorBuilder>();
	}

	void OpenGLEngine::waitForGPU() {
		glFinish();
	}

	void OpenGLEngine::destroy() {
		glDeleteFramebuffers(1, &mFramebufferObject);
		mRenderImage = nullptr;
	}

	void OpenGLEngine::initOpenGL(window::Window& window) {
		if (gladLoadGL() == 0) {
			throw std::runtime_error("Failed to initialise GLAD");
		}

#ifdef VG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum, GLenum, const GLuint id, const GLenum severity, GLsizei, const GLchar* message, const void*) {
			switch (severity) {
				case GL_DEBUG_SEVERITY_LOW:
					std::cerr << "OpenGL Low Severity Warning: id: ";
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					std::cerr << "OpenGL Warning: id: ";
					break;
				case GL_DEBUG_SEVERITY_HIGH:
					std::cerr << "OpenGL Error: id: ";
					break;
				default:
					std::cerr << "OpenGL Message: id: ";
					break;
			}
			std::cerr << id << ", message: " << message << std::endl;
		}, nullptr);
#endif

		const glm::uvec2 windowSize = window.getSize();

		glViewport(0, 0, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));
		window.setResizeCallback([](const glm::uvec2 size) {
			glViewport(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
		});

		mRenderImage = std::make_unique<OpenGLImage>(glm::uvec3{windowSize, 1}, ImageFormat::RGBA16_SFLOAT, ImageUsage::NONE, ImageType::IMAGE_2D);

		glGenFramebuffers(1, &mFramebufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderImage->getImage(), 0);
	}

	void OpenGLEngine::preRender() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
	}

	void OpenGLEngine::postRender(window::Window& window) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferObject);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		const auto windowSize = glm::ivec2(window.getSize());
		glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		window.swapOpenGLBuffers();
	}
}
