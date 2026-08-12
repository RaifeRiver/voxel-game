#pragma once

#include "glm/vec3.hpp"

namespace voxel_game::client::render {
	enum class ImageFormat {
		UNKNOWN,
		RGBA8_SRGB,
		RGBA16_SFLOAT,
		D32_SFLOAT
	};


	namespace ImageUsage {
		enum ImageUsage {
			NONE = 0,
			TRANSFER_SRC = 1 << 0,
			TRANSFER_DST = 1 << 1,
			SAMPLED = 1 << 2,
			STORAGE = 1 << 3,
			COLOUR_ATTACHMENT = 1 << 4,
			DEPTH_STENCIL_ATTACHMENT = 1 << 5
		};
	}

	enum class ImageType {
		IMAGE_1D,
		IMAGE_2D,
		IMAGE_3D
	};

	class GPUImage {
	public:
		GPUImage(glm::uvec3 size, ImageFormat format, ImageUsage::ImageUsage usage, ImageType type);

		[[nodiscard]] const glm::uvec3& getSize() const {
			return mSize;
		}

		[[nodiscard]] ImageFormat getFormat() const {
			return mFormat;
		}

		[[nodiscard]] ImageUsage::ImageUsage getUsage() const {
			return mUsage;
		}

		[[nodiscard]] ImageType getType() const {
			return mType;
		}

		virtual ~GPUImage() = default;

	protected:
		glm::uvec3 mSize{};
		ImageFormat mFormat = ImageFormat::UNKNOWN;
		ImageUsage::ImageUsage mUsage = ImageUsage::NONE;
		ImageType mType = ImageType::IMAGE_2D;
	};
}