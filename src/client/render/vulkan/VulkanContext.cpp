#include "VulkanContext.h"

#include "VulkanUtil.h"
#include "client/window/Window.h"

namespace voxel_game::client::render::vulkan {
	VulkanContext::VulkanContext(ecs::ECSRegistry& registry) {
		createInstance(registry);
	}

	void VulkanContext::createInstance(ecs::ECSRegistry& registry) {
		const VkApplicationInfo applicationInfo = vulkan_util::applicationInfo("Voxel Game", VK_API_VERSION_1_3);

		auto& window = registry.getResource<window::Window>();
		const std::vector<const char*> extensions = window.getRequiredVulkanExtensions();

		const VkInstanceCreateInfo instanceCreateInfo = vulkan_util::instanceCreateInfo(applicationInfo, extensions);
		vulkan_util::vkCheck(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));
	}
}
