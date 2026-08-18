#pragma once

#include <memory>
#include <string>

#include "glm/vec3.hpp"

#include "ComputePipeline.h"
#include "DescriptorAllocator.h"
#include "GPUImage.h"
#include "common/ecs/Resource.h"

namespace voxel_game::client::render {
	class RenderEngine : public ecs::Resource<RenderEngine> {
	public:
		virtual std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) = 0;

		virtual std::unique_ptr<ComputePipeline> createComputePipeline(const std::string& computeShader) = 0;

		virtual std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() = 0;

		virtual GPUImage& getRenderImage() = 0;
	};
}
