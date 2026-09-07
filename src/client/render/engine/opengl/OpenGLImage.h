/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
