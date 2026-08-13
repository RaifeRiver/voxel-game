#pragma once

#include "glm/vec3.hpp"

#include "GPUImage.h"
#include "common/ecs/Resource.h"

namespace voxel_game::client::render {
	class RenderEngine : public ecs::Resource<RenderEngine> {
	public:
		virtual GPUImage* allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) = 0;
	};
}
