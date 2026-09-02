#pragma once

#include "glad/glad.h"

#include "client/render/engine/GPUImage.h"

namespace voxel_game::client::render::engine::opengl {
	GLint toOpenGLImageFormat(ImageFormat format);

	GLint toOpenGLImageType(ImageType type);

	class OpenGLImage : public GPUImage {
	public:
		OpenGLImage(const glm::uvec3& size, ImageFormat format, ImageUsage usage, ImageType type);

		void transition(ImageUsage usage) override;

		void clearColour(glm::vec4 colour) override;

		[[nodiscard]] GLuint getImage() const {
			return mImage;
		}

		~OpenGLImage() override;

	private:
		GLuint mImage = 0;
	};
}
