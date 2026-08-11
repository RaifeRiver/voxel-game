#include "VulkanContext.h"

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VulkanUtil.h"
#include "client/window/Window.h"

namespace voxel_game::client::render::vulkan {
	VulkanContext::VulkanContext(ecs::ECSRegistry& registry) {
		auto& window = registry.getResource<window::Window>();

		createInstance(window);
		selectPhysicalDevice();
		createDevice(window);
		createAllocator();
		createSurface(window);
		createSwapchain(window);

		window.setVisible(true);
	}

	void VulkanContext::destroy() {
		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

		for (const VkImageView& mSwapchainImageView: mSwapchainImageViews) {
			vkDestroyImageView(mDevice, mSwapchainImageView, nullptr);
		}

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyDevice(mDevice, nullptr);

		vkDestroyInstance(mInstance, nullptr);
	}

	void VulkanContext::createInstance(window::Window& window) {
		const VkApplicationInfo applicationInfo = vulkan_util::applicationInfo("Voxel Game", VK_API_VERSION_1_3);

		const std::vector<const char*> extensions = window.getRequiredVulkanExtensions();

		std::vector<const char*> layers;
		// ReSharper disable once CppRedundantBooleanExpressionArgument
		if (ENABLE_VALIDATION_LAYERS && checkValidationLayerSupport()) {
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		const VkInstanceCreateInfo instanceCreateInfo = vulkan_util::instanceCreateInfo(applicationInfo, extensions, layers);
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

	void VulkanContext::createDevice(window::Window& window) {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

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
			.device = mDevice,
			.pVulkanFunctions = &functions,
			.instance = mInstance
		};
		vulkan_util::vkCheck(vmaCreateAllocator(&allocatorCreateInfo, &mAllocator));
	}

	void VulkanContext::createSurface(window::Window& window) {
		vulkan_util::vkCheck(window.createVulkanSurface(mInstance, &mSurface));
	}

	void VulkanContext::createSwapchain(window::Window& window) {
		VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
		vulkan_util::vkCheck(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &surfaceCapabilities));

		VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;
		const glm::uvec2 windowSize = window.getSize();
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF) {
			swapchainExtent.width = windowSize.x;
			swapchainExtent.height = windowSize.y;
		}

		constexpr VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
		const VkSwapchainCreateInfoKHR swapchainCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = mSurface,
			.minImageCount = surfaceCapabilities.minImageCount,
			.imageFormat = imageFormat,
			.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			.imageExtent = swapchainExtent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = VK_PRESENT_MODE_FIFO_KHR
		};
		vulkan_util::vkCheck(vkCreateSwapchainKHR(mDevice, &swapchainCreateInfo, nullptr, &mSwapchain));

		uint32_t imageCount = 0;
		vulkan_util::vkCheck(vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr));
		mSwapchainImages.resize(imageCount);
		vulkan_util::vkCheck(vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data()));
		mSwapchainImageViews.resize(imageCount);

		const std::vector depthFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT};
		VkFormat depthFormat = VK_FORMAT_UNDEFINED;
		for (const VkFormat format: depthFormats) {
			VkFormatProperties2 formatProperties = {.sType = VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2};
			vkGetPhysicalDeviceFormatProperties2(mPhysicalDevice, format, &formatProperties);
			if (formatProperties.formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				depthFormat = format;
				break;
			}
		}

		VkImageCreateInfo depthImageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = depthFormat,
			.extent = {.width = windowSize.x, .height = windowSize.y, .depth = 1},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};
		VmaAllocationCreateInfo allocationCreateInfo = {
			.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
			.usage = VMA_MEMORY_USAGE_AUTO
		};
		vulkan_util::vkCheck(vmaCreateImage(mAllocator, &depthImageCreateInfo, &allocationCreateInfo, &mDepthImage, &mDepthImageAllocation, nullptr));

		VkImageViewCreateInfo depthImageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = mDepthImage,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = depthFormat,
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
				.levelCount = 1,
				.layerCount = 1
			}
		};
		vulkan_util::vkCheck(vkCreateImageView(mDevice, &depthImageViewCreateInfo, nullptr, &mDepthImageView));
	}

	bool VulkanContext::checkValidationLayerSupport() {
		uint32_t layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> layerProperties(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
		for (const VkLayerProperties layerProperty : layerProperties) {
			if (strcmp(layerProperty.layerName, "VK_LAYER_KHRONOS_validation") == 0) {
				return true;
			}
		}
		return false;
	}
}
