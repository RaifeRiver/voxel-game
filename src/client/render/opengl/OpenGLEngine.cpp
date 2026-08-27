#include "OpenGLEngine.h"

#include <cstring>
#include <iostream>
#include <stdexcept>

#include "OpenGLBuffer.h"
#include "glad/glad.h"

#include "OpenGLComputePipeline.h"
#include "OpenGLDescriptorAllocator.h"
#include "OpenGLImage.h"
#include "OpenGLRenderPipeline.h"
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

	std::unique_ptr<GPUImage> OpenGLEngine::allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) {
		return std::make_unique<OpenGLImage>(size, format, usage, type);
	}

	std::unique_ptr<ComputePipeline> OpenGLEngine::createComputePipeline(const std::string& computeShader) {
		return std::make_unique<OpenGLComputePipeline>(computeShader);
	}

	std::unique_ptr<RenderPipelineBuilder> OpenGLEngine::createRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) {
		return std::make_unique<OpenGLRenderPipelineBuilder>(vertexShader, fragmentShader);
	}

	std::unique_ptr<DescriptorAllocatorBuilder> OpenGLEngine::createDescriptorAllocatorBuilder() {
		return std::make_unique<OpenGLDescriptorAllocatorBuilder>();
	}

	void OpenGLEngine::beginRendering() {}

	void OpenGLEngine::endRendering() {}

	void OpenGLEngine::waitForGPU() {
		glFinish();
	}

	void OpenGLEngine::destroy() {
		glDeleteFramebuffers(1, &mFramebufferObject);
		mRenderImage = nullptr;
	}

	std::unique_ptr<GPUBuffer> OpenGLEngine::allocateBuffer_(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType) {
		return std::make_unique<OpenGLBuffer>(size, usage, memoryType, mappedType);
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

	void OpenGLEngine::preRender() {
		const uint64_t frame = mFrame % FRAME_OVERLAP;

		if (mRenderFences[frame] != nullptr) {
			glClientWaitSync(mRenderFences[frame], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
			glDeleteSync(mRenderFences[frame]);
			mRenderFences[frame] = nullptr;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferObject);
	}

	void OpenGLEngine::postRender(window::Window& window) {
		const uint64_t frame = mFrame % FRAME_OVERLAP;

		glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferObject);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		const auto windowSize = glm::ivec2(window.getSize());
		glBlitFramebuffer(0, windowSize.y, windowSize.x, 0, 0, 0, windowSize.x, windowSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		mRenderFences[frame] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

		window.swapOpenGLBuffers();
	}
}
