#include "OpenGLEngine.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glad/glad.h"

#include "OpenGLComputePipeline.h"
#include "OpenGLDescriptorAllocator.h"
#include "OpenGLImage.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::opengl {
	OpenGLEngine::OpenGLEngine(ecs::ECSRegistry& registry) {
		LOG_INFO("Using OpenGL renderer");

		auto& window = registry.getResource<window::Window>();

		initOpenGL(window);

		registry.getSystemManager().registerSystem(ecs::Stage::PRE_RENDER, [this](ecs::ECSRegistry& r, float) {
			preRender();
		});
		registry.getSystemManager().registerSystem(ecs::Stage::POST_RENDER, [this](ecs::ECSRegistry& r, float) {
			postRender(r.getResource<window::Window>());
		});

		LOG_INFO("OpenGL renderer initialised");
	}

	std::unique_ptr<GPUImage> OpenGLEngine::allocateImage(glm::ivec3, ImageFormat, ImageUsage, ImageType) {
		throw std::runtime_error("OpenGL image allocation is not implemented");
	}

	std::unique_ptr<ComputePipeline> OpenGLEngine::createComputePipeline(const std::string& computeShader) {
		return std::make_unique<OpenGLComputePipeline>(computeShader);
	}

	std::unique_ptr<RenderPipelineBuilder> OpenGLEngine::createRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) {
		throw std::runtime_error("OpenGL render pipelines are not implemented");
	}

	std::unique_ptr<DescriptorAllocatorBuilder> OpenGLEngine::createDescriptorAllocatorBuilder() {
		return std::make_unique<OpenGLDescriptorAllocatorBuilder>();
	}

	void OpenGLEngine::beginRendering() {
		throw std::runtime_error("OpenGL render pipelines are not implemented");
	}

	void OpenGLEngine::endRendering() {
		throw std::runtime_error("OpenGL render pipelines are not implemented");
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
			LOG_FATAL("Failed to initialize GLAD");
			throw std::runtime_error("Failed to initialise GLAD");
		}

#ifdef VG_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum, GLenum, const GLuint id, const GLenum severity, GLsizei, const GLchar* message, const void*) {
			const size_t messageLength = strlen(message);
			std::string_view trimmedMessage(message, message[messageLength - 1] == '\n'? messageLength - 1 : messageLength);
			switch (severity) {
				case GL_DEBUG_SEVERITY_LOW:
					LOG_INFO("OpenGL Low Severity Warning: id: {}, message: {}", id, trimmedMessage);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					LOG_WARNING("OpenGL Warning: id: {}, message: {}", id, trimmedMessage);
					break;
				case GL_DEBUG_SEVERITY_HIGH:
					LOG_ERROR("OpenGL Error: id: {}, message: {}", id, trimmedMessage);
					break;
				default:
					LOG_INFO("OpenGL Message: id: {}, message: {}", id, trimmedMessage);
					break;
			}
		}, nullptr);
#endif

		const glm::uvec2 windowSize = window.getSize();

		glViewport(0, 0, static_cast<int>(windowSize.x), static_cast<int>(windowSize.y));
		window.setResizeCallback([this](const glm::uvec2 size) {
			glViewport(0, 0, static_cast<int>(size.x), static_cast<int>(size.y));
			mRenderImage = std::make_unique<OpenGLImage>(glm::uvec3{size, 1}, ImageFormat::RGBA16_SFLOAT, ImageUsage::NONE, ImageType::IMAGE_2D);
			glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderImage->getImage(), 0);
		});

		LOG_DEBUG("Creating framebuffer image");

		mRenderImage = std::make_unique<OpenGLImage>(glm::uvec3{windowSize, 1}, ImageFormat::RGBA16_SFLOAT, ImageUsage::NONE, ImageType::IMAGE_2D);

		glGenFramebuffers(1, &mFramebufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mRenderImage->getImage(), 0);

		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void OpenGLEngine::preRender() const {
		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
	}

	void OpenGLEngine::postRender(window::Window& window) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferObject);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		const auto windowSize = glm::ivec2(window.getSize());
		glBlitFramebuffer(0, windowSize.y, windowSize.x, 0, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		window.swapOpenGLBuffers();
	}
}
