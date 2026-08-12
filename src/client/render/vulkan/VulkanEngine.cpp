#include "VulkanEngine.h"

#include <cmath>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "VulkanUtil.h"
#include "client/window/Window.h"

namespace voxel_game::client::render::vulkan {
	VulkanEngine::VulkanEngine(ecs::ECSRegistry& registry) {
		auto& window = registry.getResource<window::Window>();

		createInstance(window);
		selectPhysicalDevice();
		createDevice(window);
		createAllocator();
		createSurface(window);
		createSwapchain(window);
		createCommandBuffers();
		createSyncStructures();

		window.setVisible(true);

		registry.getSystemManager().registerSystem(ecs::Stage::PRE_RENDER, [this](ecs::ECSRegistry&, float) {
			preRender();
		});
		registry.getSystemManager().registerSystem(ecs::Stage::POST_RENDER, [this](ecs::ECSRegistry&, float) {
			postRender();
		});
	}

	void VulkanEngine::destroy() {
		vkDeviceWaitIdle(mDevice);

		// ReSharper disable once CppLocalVariableMayBeConst
		for (VkSemaphore semaphore: mRenderSemaphores) {
			vkDestroySemaphore(mDevice,semaphore, nullptr);
		}

		for (const VulkanFrameData& frameData: mFrameData) {
			vkDestroyCommandPool(mDevice, frameData.commandPool, nullptr);

			vkDestroyFence(mDevice, frameData.renderFence, nullptr);
			vkDestroySemaphore(mDevice, frameData.swapchainSemaphore, nullptr);
		}

		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

		for (const VkImageView& mSwapchainImageView: mSwapchainImageViews) {
			vkDestroyImageView(mDevice, mSwapchainImageView, nullptr);
		}

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyDevice(mDevice, nullptr);

		vkDestroyInstance(mInstance, nullptr);
	}

	void VulkanEngine::createInstance(window::Window& window) {
		const VkApplicationInfo applicationInfo = vulkan_util::applicationInfo("Voxel Game", VK_API_VERSION_1_3);

		const std::vector<const char*> extensions = window.getRequiredVulkanExtensions();

		std::vector<const char*> layers;
		// ReSharper disable once CppRedundantBooleanExpressionArgument
		// ReSharper disable once CppIfCanBeReplacedByConstexprIf
		if (ENABLE_VALIDATION_LAYERS && checkValidationLayerSupport()) {
			layers.push_back("VK_LAYER_KHRONOS_validation");
		}

		const VkInstanceCreateInfo instanceCreateInfo = vulkan_util::instanceCreateInfo(applicationInfo, extensions, layers);
		vulkan_util::vkCheck(vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance));
	}

	void VulkanEngine::selectPhysicalDevice() {
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

	void VulkanEngine::createDevice(window::Window& window) {
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
		features13.synchronization2 = true;

		VkPhysicalDeviceFeatures features = {};

		VkDeviceCreateInfo deviceCreateInfo = vulkan_util::deviceCreateInfo(1, &deviceQueueCreateInfo, extensions, &features, &features13);
		vulkan_util::vkCheck(vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice));

		vkGetDeviceQueue(mDevice, queueFamily, 0, &mGraphicsQueue);
		mGraphicsQueueFamily = queueFamily;
	}

	void VulkanEngine::createAllocator() {
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

	void VulkanEngine::createSurface(window::Window& window) {
		vulkan_util::vkCheck(window.createVulkanSurface(mInstance, &mSurface));
	}

	void VulkanEngine::createSwapchain(window::Window& window) {
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
			.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
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
	}

	void VulkanEngine::createCommandBuffers() {
		const VkCommandPoolCreateInfo commandPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = mGraphicsQueueFamily,
		};
		VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};
		for (VulkanFrameData& frameData: mFrameData) {
			vulkan_util::vkCheck(vkCreateCommandPool(mDevice, &commandPoolCreateInfo, nullptr, &frameData.commandPool));

			commandBufferAllocateInfo.commandPool = frameData.commandPool;
			vulkan_util::vkCheck(vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &frameData.commandBuffer));
		}
	}

	void VulkanEngine::createSyncStructures() {
		constexpr VkFenceCreateInfo fenceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT,
		};
		constexpr VkSemaphoreCreateInfo semaphoreCreateInfo = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};

		for (VulkanFrameData& frameData: mFrameData) {
			vulkan_util::vkCheck(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &frameData.swapchainSemaphore));

			vulkan_util::vkCheck(vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &frameData.renderFence));
		}

		mRenderSemaphores.resize(mSwapchainImages.size());
		for (VkSemaphore& renderSemaphore: mRenderSemaphores) {
			vulkan_util::vkCheck(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &renderSemaphore));
		}
	}

	void VulkanEngine::preRender() {
		const VulkanFrameData& frameData = getFrameData();

		vulkan_util::vkCheck(vkWaitForFences(mDevice, 1, &frameData.renderFence, true, 1000000000));
		vulkan_util::vkCheck(vkResetFences(mDevice, 1, &frameData.renderFence));

		vulkan_util::vkCheck(vkAcquireNextImageKHR(mDevice, mSwapchain, 1000000000, frameData.swapchainSemaphore, nullptr, &mCurrentSwapchainIndex), false);

		// ReSharper disable once CppLocalVariableMayBeConst
		VkCommandBuffer commandBuffer = frameData.commandBuffer;
		vulkan_util::vkCheck(vkResetCommandBuffer(commandBuffer, 0));

		constexpr VkCommandBufferBeginInfo commandBufferBeginInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
		};
		vulkan_util::vkCheck(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

		vulkan_util::transitionImage(commandBuffer, mSwapchainImages[mCurrentSwapchainIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		const VkClearColorValue clearColorValue = {std::abs(std::sin(static_cast<float>(mFrame) / 300.0f)), 0.0f, 0.0f, 1.0f};
		constexpr VkImageSubresourceRange subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS,
		};
		vkCmdClearColorImage(commandBuffer, mSwapchainImages[mCurrentSwapchainIndex], VK_IMAGE_LAYOUT_GENERAL, &clearColorValue, 1, &subresourceRange);
	}

	void VulkanEngine::postRender() {
		const VulkanFrameData& frameData = getFrameData();
		// ReSharper disable once CppLocalVariableMayBeConst
		VkCommandBuffer commandBuffer = frameData.commandBuffer;

		vulkan_util::transitionImage(commandBuffer, mSwapchainImages[mCurrentSwapchainIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		vulkan_util::vkCheck(vkEndCommandBuffer(commandBuffer));

		VkCommandBufferSubmitInfo commandBufferSubmitInfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = commandBuffer
		};
		VkSemaphoreSubmitInfo waitInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = frameData.swapchainSemaphore,
			.value = 1,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR
		};
		VkSemaphoreSubmitInfo signalInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = mRenderSemaphores[mCurrentSwapchainIndex],
			.value = 1,
			.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT
		};
		const VkSubmitInfo2 submitInfo = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &waitInfo,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &commandBufferSubmitInfo,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &signalInfo
		};
		vulkan_util::vkCheck(vkQueueSubmit2(mGraphicsQueue, 1, &submitInfo, frameData.renderFence));

		const VkPresentInfoKHR presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &mRenderSemaphores[mCurrentSwapchainIndex],
			.swapchainCount = 1,
			.pSwapchains = &mSwapchain,
			.pImageIndices = &mCurrentSwapchainIndex,
		};
		vulkan_util::vkCheck(vkQueuePresentKHR(mGraphicsQueue, &presentInfo), false);

		mFrame++;
	}

	bool VulkanEngine::checkValidationLayerSupport() {
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
