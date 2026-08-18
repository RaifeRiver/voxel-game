#include "OpenGLRenderImage.h"

#include <stdexcept>

#include "glad/glad.h"

namespace voxel_game::client::render::opengl {
	OpenGLRenderImage::OpenGLRenderImage(const glm::uvec3 &size) : GPUImage(size, ImageFormat::RGBA8_SRGB, ImageUsage::NONE, ImageType::IMAGE_2D) {}

	void OpenGLRenderImage::transition(ImageUsage usage) {
		throw std::runtime_error("Not implemented");
	}

	void OpenGLRenderImage::clearColour(const glm::vec4 colour) {
		glClearColor(colour.r, colour.g, colour.b, colour.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}
