#include "VulkanContext.h"

#include "VulkanUtil.h"
#include "client/window/Window.h"

namespace voxel_game::client::render::vulkan {
	VulkanContext::VulkanContext(ecs::ECSRegistry& registry) {
		createInstance(registry);
		selectPhysicalDevice();
		createDevice(registry);
		createAllocator();
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

	void VulkanContext::createDevice(ecs::ECSRegistry& registry) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

		auto& window = registry.getResource<window::Window>();

		bool foundQueueFamily = false;
		uint32_t queueFamily = 0;
		for (size_t i = 0; i < queueFamilies.size(); i++) {
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && window.getVulkanPhysicalDevicePresentationSupport(mInstance, mPhysicalDevice, i)) {
				foundQueueFamily = true;
				queueFamily = i;
				break;
			}
		}

		if (!foundQueueFamily) {
			throw std::runtime_error("No suitable Vulkan queue found");
		}

		const std::vector extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

		constexpr float queuePriorities = 1.0f;
		VkDeviceQueueCreateInfo deviceQueueCreateInfo = vulkan_util::deviceQueueCreateInfo(queueFamily, &queuePriorities);

		VkPhysicalDeviceVulkan12Features features12 = {};
		features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

		VkPhysicalDeviceVulkan13Features features13 = {};
		features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.pNext = &features12;

		VkPhysicalDeviceFeatures features = {};

		VkDeviceCreateInfo deviceCreateInfo = vulkan_util::deviceCreateInfo(1, &deviceQueueCreateInfo, extensions, &features, &features13);
		vulkan_util::vkCheck(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));

		vkGetDeviceQueue(mDevice, queueFamily, 0, &mQueue);
	}

	void VulkanContext::createAllocator() {
		VmaVulkanFunctions functions = vulkan_util::vmaVulkanFunctions();
		const VmaAllocatorCreateInfo allocatorCreateInfo = {
			.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
			.physicalDevice = mPhysicalDevice,
			.pVulkanFunctions = &functions,
			.instance = mInstance
		};
		vulkan_util::vkCheck(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
	}
}
