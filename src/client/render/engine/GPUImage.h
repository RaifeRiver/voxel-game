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

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

namespace voxel_game::client::render::engine {
	enum class ImageFormat {
		UNKNOWN,
		RGBA8_SRGB,
		RGBA16_SFLOAT,
		D32_SFLOAT
	};

	bool isColourFormat(ImageFormat imageFormat);

	bool isDepthFormat(ImageFormat imageFormat);

	enum class ImageUsage : uint32_t {
		NONE = 0,
		TRANSFER_SRC = 1 << 0,
		TRANSFER_DST = 1 << 1,
		SAMPLED = 1 << 2,
		STORAGE = 1 << 3,
		COLOUR_ATTACHMENT = 1 << 4,
		DEPTH_ATTACHMENT = 1 << 5
	};

	inline ImageUsage operator|(ImageUsage a, ImageUsage b) {
		return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline uint32_t operator&(ImageUsage a, ImageUsage b) {
		return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
	}

	enum class ImageType {
		IMAGE_1D,
		IMAGE_2D,
		IMAGE_3D
	};

	class GPUImage {
	public:
		GPUImage(glm::uvec3 size, ImageFormat format, ImageUsage usage, ImageType type);

		virtual void transition(ImageUsage usage) = 0;

		virtual void clearColour(glm::vec4 colour) = 0;

		[[nodiscard]] const glm::uvec3& getSize() const {
			return mSize;
		}

		[[nodiscard]] ImageFormat getFormat() const {
			return mFormat;
		}

		[[nodiscard]] ImageUsage getUsage() const {
			return mUsage;
		}

		[[nodiscard]] ImageType getType() const {
			return mType;
		}

		virtual ~GPUImage() = default;

	protected:
		glm::uvec3 mSize{};
		ImageFormat mFormat = ImageFormat::UNKNOWN;
		ImageUsage mUsage = ImageUsage::NONE;
		ImageType mType = ImageType::IMAGE_2D;
	};
}