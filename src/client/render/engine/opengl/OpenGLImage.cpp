#include "OpenGLImage.h"

#include <stdexcept>

namespace voxel_game::client::render::engine::opengl {
	GLint toOpenGLImageFormat(const ImageFormat format) {
		switch (format) {
			case ImageFormat::RGBA8_SRGB:
				return GL_RGBA8;
			case ImageFormat::RGBA16_SFLOAT:
				return GL_RGBA16F;
			case ImageFormat::D32_SFLOAT:
				return GL_DEPTH_COMPONENT32F;
			default:
				throw std::runtime_error("Unsupported image format");
		}
	}

	GLint toOpenGLImageType(const ImageType type) {
		switch (type) {
			case ImageType::IMAGE_1D:
				return GL_TEXTURE_1D;
			case ImageType::IMAGE_2D:
				return GL_TEXTURE_2D;
			case ImageType::IMAGE_3D:
				return GL_TEXTURE_3D;
			default:
				throw std::runtime_error("Unsupported image type");
		}
	}

	OpenGLImage::OpenGLImage(const glm::uvec3& size, const ImageFormat format, const ImageUsage usage, const ImageType type) : GPUImage(size, format, usage, type) {
		glGenTextures(1, &mImage);
		const GLint glType = toOpenGLImageType(type);
		glBindTexture(glType, mImage);
		glTexParameteri(glType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(glType, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(glType, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(glType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		const int glInternalFormat = toOpenGLImageFormat(format);
		const int glFormat = isDepthFormat(format)? GL_DEPTH_COMPONENT: GL_RGBA;
		switch (type) {
			case ImageType::IMAGE_1D:
				glTexImage1D(glType, 0, glInternalFormat, static_cast<GLint>(size.x), 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
				break;
			case ImageType::IMAGE_2D:
				glTexImage2D(glType, 0, glInternalFormat, static_cast<GLint>(size.x), static_cast<GLint>(size.y), 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
				break;
			case ImageType::IMAGE_3D:
				glTexImage3D(glType, 0, glInternalFormat, static_cast<GLint>(size.x), static_cast<GLint>(size.y), static_cast<GLint>(size.z), 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
				break;
		}
	}

	void OpenGLImage::transition(ImageUsage) {}

	void OpenGLImage::clearColour(glm::vec4) {
		throw std::runtime_error("OpenGL image clearing is not supported");
	}

	OpenGLImage::~OpenGLImage() {
		glDeleteTextures(1, &mImage);
	}
}
