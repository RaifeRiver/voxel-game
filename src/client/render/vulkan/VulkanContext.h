#pragma once

#include <vulkan/vulkan.h>

#include "client/render/RenderContext.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::vulkan {
	class VulkanContext : public RenderContext {
	public:
		explicit VulkanContext(ecs::ECSRegistry& registry);

	private:
		VkInstance mInstance = nullptr;
		VkPhysicalDevice mPhysicalDevice = nullptr;

		void createInstance(ecs::ECSRegistry& registry);

		void selectPhysicalDevice();
	};
}
