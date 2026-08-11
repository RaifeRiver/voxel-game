#include "VulkanContext.h"

#include "VulkanUtil.h"
#include "client/window/Window.h"

namespace voxel_game::client::render::vulkan {
	VulkanContext::VulkanContext(ecs::ECSRegistry& registry) {
		createInstance(registry);
		selectPhysicalDevice();
	}

	void VulkanContext::createInstance(ecs::ECSRegistry& registry) {
		const VkApplicationInfo applicationInfo = vulkan_util::applicationInfo("Voxel Game", VK_API_VERSION_1_3);

		auto& window = registry.getResource<window::Window>();
		const std::vector<const char*> extensions = window.getRequiredVulkanExtensions();

		const VkInstanceCreateInfo instanceCreateInfo = vulkan_util::instanceCreateInfo(applicationInfo, extensions);
		vulkan_util::vkCheck(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));
	}

	void VulkanContext::selectPhysicalDevice() {
		uint32_t deviceCount = 0;
		vulkan_util::vkCheck(vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr));
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vulkan_util::vkCheck(vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data()));

		int32_t deviceScore = -1;
		VkPhysicalDevice device;
		std::string deviceName;
		for (VkPhysicalDevice d : devices) {
			int32_t score = 0;

			VkPhysicalDeviceProperties2 properties = {.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
			vkGetPhysicalDeviceProperties2(d, &properties);

			VkPhysicalDeviceType deviceType = properties.properties.deviceType;
			if (deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				score += 1000;
			}
			else if (deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
				score += 100;
			}

			if (deviceScore < score) {
				deviceScore = score;
				device = d;
				deviceName = std::string(properties.properties.deviceName);
			}
		}

		if (deviceScore == -1) {
			throw std::runtime_error("No suitable Vulkan devices found");
		}

		mPhysicalDevice = device;
		std::cout << "Using Vulkan device: " << deviceName << std::endl;
	}
}
