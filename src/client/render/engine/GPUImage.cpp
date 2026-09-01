#include "GPUImage.h"

namespace voxel_game::client::render {
	bool isColourFormat(const ImageFormat imageFormat) {
		return imageFormat == ImageFormat::RGBA8_SRGB || imageFormat == ImageFormat::RGBA16_SFLOAT;
	}

	bool isDepthFormat(const ImageFormat imageFormat) {
		return imageFormat == ImageFormat::D32_SFLOAT;
	}

	GPUImage::GPUImage(const glm::uvec3 size, const ImageFormat format, const ImageUsage usage, const ImageType type) : mSize(size), mFormat(format), mUsage(usage), mType(type) {}
}
