#include "GPUImage.h"

namespace voxel_game::client::render {
	GPUImage::GPUImage(const glm::uvec3 size, const ImageFormat format, const ImageUsage usage, const ImageType type) : mSize(size), mFormat(format), mUsage(usage), mType(type) {}
}
