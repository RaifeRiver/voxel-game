#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "common/ecs/Resource.h"

namespace voxel_game::client::window {
	class Window : public ecs::Resource<Window> {
	public:
		virtual std::vector<const char*> getRequiredVulkanExtensions() = 0;

		virtual bool getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamily) = 0;
	};
}
