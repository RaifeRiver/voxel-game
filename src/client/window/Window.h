#pragma once

#include <vector>

#include "common/ecs/Resource.h"

namespace voxel_game::client::window {
	class Window : public ecs::Resource<Window> {
	public:
		virtual std::vector<const char*> getRequiredVulkanExtensions() = 0;
	};
}
