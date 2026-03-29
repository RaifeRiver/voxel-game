#include "RenderContext.h"

#include <iostream>

#include "tracy/TracyVulkan.hpp"

#include "DescriptorSetLayoutBuilder.h"
#define VMA_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include "vk_mem_alloc.h"
#include "VkBootstrap.h"
#include "VulkanInitialisers.h"
#include "VulkanUtil.h"

namespace voxel_game::client::renderer {
	class WindowManager;
#ifdef VG_DEBUG
	constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
	constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

	void FrameContext::initCommands(const RenderContext* context, const VkCommandPoolCreateInfo &commandPoolInfo) {
		VK_CHECK(vkCreateCommandPool(context->getDevice(), &commandPoolInfo, nullptr, &mCommandPool));
		const VkCommandBufferAllocateInfo commandBufferAllocateInfo = initialisers::commandBufferAllocateInfo(mCommandPool, 1);
		VK_CHECK(vkAllocateCommandBuffers(context->getDevice(), &commandBufferAllocateInfo, &mCommandBuffer));
	}

	void FrameContext::initSyncStructures(const RenderContext* context, const VkFenceCreateInfo &fenceCreateInfo, const VkSemaphoreCreateInfo &semaphoreCreateInfo) {
		VK_CHECK(vkCreateFence(context->getDevice(), &fenceCreateInfo, nullptr, &mRenderFence));
		VK_CHECK(vkCreateSemaphore(context->getDevice(), &semaphoreCreateInfo, nullptr, &mSwapchainSemaphore));
	}

	void FrameContext::destroy(const RenderContext* context) const {
		vkDestroyCommandPool(context->getDevice(), mCommandPool, nullptr);

		vkDestroyFence(context->getDevice(), mRenderFence, nullptr);
		vkDestroySemaphore(context->getDevice(), mSwapchainSemaphore, nullptr);
	}

	RenderContext::RenderContext(const WindowManager* windowManager) {
		initVulkan(windowManager);
		const VkExtent2D swapchainExtent = windowManager->getSize();
		initSwapchain(swapchainExtent.width, swapchainExtent.height);
		initSyncStructures();
		initCommands();

		immediateSubmit([this](const VkCommandBuffer commandBuffer) {
			mTracyContext = TracyVkContext(mPhysicalDevice, mDevice, mGraphicsQueue, commandBuffer);
		});

		initDescriptorSets();
	}

	void RenderContext::immediateSubmit(std::function<void(VkCommandBuffer commandBuffer)> &&function) const {
		VK_CHECK(vkResetFences(mDevice, 1, &mImmediateFence));
		VK_CHECK(vkResetCommandBuffer(mImmediateCommandBuffer, 0));

		VkCommandBufferBeginInfo cmdBeginInfo = initialisers::commandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VK_CHECK(vkBeginCommandBuffer(mImmediateCommandBuffer, &cmdBeginInfo));

		function(mImmediateCommandBuffer);

		VK_CHECK(vkEndCommandBuffer(mImmediateCommandBuffer));

		VkCommandBufferSubmitInfo commandBufferSubmitInfo = initialisers::commandBufferSubmitInfo(mImmediateCommandBuffer);
		VkSubmitInfo2 submit = initialisers::submitInfo(&commandBufferSubmitInfo, nullptr, nullptr);

		VK_CHECK(vkQueueSubmit2(mGraphicsQueue, 1, &submit, mImmediateFence));

		VK_CHECK(vkWaitForFences(mDevice, 1, &mImmediateFence, true, UINT64_MAX));
	}

	void RenderContext::resizeSwapchain(const WindowManager* windowManager) {
		vkDeviceWaitIdle(mDevice);

		destroySwapchain();
		const VkExtent2D swapchainExtent = windowManager->getSize();
		initSwapchain(swapchainExtent.width, swapchainExtent.height);
	}

	void RenderContext::destroy() const {
		vkDeviceWaitIdle(mDevice);

		for (const FrameContext& frameContext: mFrameContexts) {
			frameContext.destroy(this);
		}

		vkDestroyCommandPool(mDevice, mImmediateCommandPool, nullptr);
		vkDestroyFence(mDevice, mImmediateFence, nullptr);

		for (const VkSemaphore& semaphore: mRenderSemaphores) {
			vkDestroySemaphore(mDevice, semaphore, nullptr);
		}

		mDescriptorAllocator.destroy(mDevice);

		vkDestroyDescriptorSetLayout(mDevice, mDrawImageDescriptorSetLayout, nullptr);

		vkDestroyImageView(mDevice, mDrawImage.imageView, nullptr);
		vmaDestroyImage(mAllocator, mDrawImage.image, mDrawImage.allocation);

		vmaDestroyAllocator(mAllocator);

		destroySwapchain();

		TracyVkDestroy(mTracyContext);

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyDevice(mDevice, nullptr);

		vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
		vkDestroyInstance(mInstance, nullptr);
	}

#define REQUIRE_FEATURE(x, success, message) do { \
	if (!x) { \
		std::cerr << message << std::endl; \
		success = false; \
	} \
} while (0)

	void RenderContext::initVulkan(const WindowManager* windowManager) {
		vkb::InstanceBuilder instanceBuilder;
		vkb::Result<vkb::Instance> instanceResult = instanceBuilder.set_app_name("Voxel Game").request_validation_layers(ENABLE_VALIDATION_LAYERS).use_default_debug_messenger().require_api_version(1, 3, 0).build();
		const vkb::Instance instance = instanceResult.value();
		mInstance = instance.instance;
		mDebugMessenger = instance.debug_messenger;

		windowManager->createSurface(mInstance, &mSurface);

		VkPhysicalDeviceVulkan13Features features13 = {};
		features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.dynamicRendering = true;
		features13.synchronization2 = true;

		VkPhysicalDeviceVulkan12Features features12 = {};
		features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;
		features12.shaderBufferInt64Atomics = true;
		features12.samplerFilterMinmax = true;
		features12.runtimeDescriptorArray = true;
		features12.shaderStorageBufferArrayNonUniformIndexing = true;

		VkPhysicalDeviceFeatures2 features = {};
		features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.features.shaderInt64 = true;

		vkb::PhysicalDeviceSelector selector{instance, mSurface};
		vkb::Result<std::vector<vkb::PhysicalDevice>> devicesResult = selector.select_devices();
		if (!devicesResult) {
			std::cout << "Failed to find suitable devices: " << devicesResult.error() << std::endl;
			throw std::runtime_error("Failed to find suitable devices");
		}
		std::vector<vkb::PhysicalDevice> devices = devicesResult.value();
		std::vector<vkb::PhysicalDevice> suitableDevices;
		for (const vkb::PhysicalDevice& device: devices) {
			VkPhysicalDeviceProperties properties = device.properties;
			std::cout << "Found device: " << properties.deviceName << std::endl;

			VkPhysicalDeviceVulkan13Features deviceFeatures13 = {};
			deviceFeatures13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

			VkPhysicalDeviceVulkan12Features deviceFeatures12 = {};
			deviceFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
			deviceFeatures12.pNext = &deviceFeatures13;

			VkPhysicalDeviceFeatures2 deviceFeatures = {};
			deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
			deviceFeatures.pNext = &deviceFeatures12;

			vkGetPhysicalDeviceFeatures2(device, &deviceFeatures);

			bool hasFeatures = true;

			REQUIRE_FEATURE(deviceFeatures.features.shaderInt64, hasFeatures, "Device does not support shaderInt64");

			REQUIRE_FEATURE(deviceFeatures12.bufferDeviceAddress, hasFeatures, "Device does not support bufferDeviceAddress");
			REQUIRE_FEATURE(deviceFeatures12.descriptorIndexing, hasFeatures, "Device does not support descriptorIndexing");
			REQUIRE_FEATURE(deviceFeatures12.shaderBufferInt64Atomics, hasFeatures, "Device does not support shaderBufferInt64Atomics");
			REQUIRE_FEATURE(deviceFeatures12.samplerFilterMinmax, hasFeatures, "Device does not support samplerFilterMinmax");
			REQUIRE_FEATURE(deviceFeatures12.runtimeDescriptorArray, hasFeatures, "Device does not support runtimeDescriptorArray");
			REQUIRE_FEATURE(deviceFeatures12.shaderStorageBufferArrayNonUniformIndexing, hasFeatures, "Device does not support shaderStorageBufferArrayNonUniformIndexing");

			REQUIRE_FEATURE(deviceFeatures13.dynamicRendering, hasFeatures, "Device does not support dynamicRendering");
			REQUIRE_FEATURE(deviceFeatures13.synchronization2, hasFeatures, "Device does not support synchronization2");

			if (!hasFeatures || !(properties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {
				std::cout << "Device does not satisfy requirements" << std::endl;
			}
			else {
				suitableDevices.push_back(device);
			}
		}
		if (suitableDevices.empty()) {
			throw std::runtime_error("No suitable devices found");
		}

		vkb::PhysicalDevice physicalDevice = suitableDevices[0];
		mPhysicalDevice = physicalDevice.physical_device;

		std::cout << "Using GPU: " << physicalDevice.properties.deviceName << std::endl;

		vkb::DeviceBuilder deviceBuilder{physicalDevice};
		vkb::Device device = deviceBuilder.add_pNext(&features13).add_pNext(&features12).add_pNext(&features).build().value();
		mDevice = device.device;

		vkb::Result<VkQueue> graphicsQueueResult = device.get_queue(vkb::QueueType::graphics);
		if (!graphicsQueueResult) {
			std::cerr << "Failed to get graphics queue: " << graphicsQueueResult.error() << std::endl;
			throw std::runtime_error("Failed to get graphics queue");
		}
		mGraphicsQueue = graphicsQueueResult.value();
		mGraphicsQueueFamily = device.get_queue_index(vkb::QueueType::graphics).value();

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.physicalDevice = physicalDevice.physical_device;
		allocatorCreateInfo.device = mDevice;
		allocatorCreateInfo.instance = mInstance;
		allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		vmaCreateAllocator(&allocatorCreateInfo, &mAllocator);
	}

	void RenderContext::initSwapchain(const uint32_t width, const uint32_t height) {
		vkb::SwapchainBuilder swapchainBuilder{mPhysicalDevice, mDevice, mSurface};
		mSwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
		vkb::Swapchain swapchain = swapchainBuilder.set_desired_format(VkSurfaceFormatKHR{mSwapchainImageFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR).set_desired_extent(width, height).add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT).build().value();
		mSwapchain = swapchain.swapchain;
		mSwapchainImages = swapchain.get_images().value();
		mSwapchainImageViews = swapchain.get_image_views().value();
		mSwapchainExtent = swapchain.extent;

		const VkExtent3D drawImageExtent = {mSwapchainExtent.width, mSwapchainExtent.height, 1};
		mDrawImage.extent = drawImageExtent;
		mDrawImage.format = VK_FORMAT_R16G16B16A16_SFLOAT;

		const VkImageCreateInfo imageCreateInfo = initialisers::imageCreateInfo(mDrawImage.format, drawImageExtent, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

		VmaAllocationCreateInfo allocationCreateInfo = {};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocationCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		vmaCreateImage(mAllocator, &imageCreateInfo, &allocationCreateInfo, &mDrawImage.image, &mDrawImage.allocation, nullptr);

		const VkImageViewCreateInfo imageViewCreateInfo = initialisers::imageViewCreateInfo(mDrawImage.image, mDrawImage.format, VK_IMAGE_ASPECT_COLOR_BIT);
		VK_CHECK(vkCreateImageView(mDevice, &imageViewCreateInfo, nullptr, &mDrawImage.imageView));
	}

	void RenderContext::initCommands() {
		const VkCommandPoolCreateInfo commandPoolInfo =  initialisers::commandPoolCreateInfo(mGraphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		for (FrameContext& frameContext: mFrameContexts) {
			frameContext.initCommands(this, commandPoolInfo);
		}

		VK_CHECK(vkCreateCommandPool(mDevice, &commandPoolInfo, nullptr, &mImmediateCommandPool));

		VkCommandBufferAllocateInfo commandBufferAllocateInfo = initialisers::commandBufferAllocateInfo(mImmediateCommandPool, 1);
		VK_CHECK(vkAllocateCommandBuffers(mDevice, &commandBufferAllocateInfo, &mImmediateCommandBuffer));
	}

	void RenderContext::initSyncStructures() {
		const VkFenceCreateInfo fenceCreateInfo = initialisers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
		const VkSemaphoreCreateInfo semaphoreCreateInfo = initialisers::semaphoreCreateInfo();
		for (FrameContext& frameContext: mFrameContexts) {
			frameContext.initSyncStructures(this, fenceCreateInfo, semaphoreCreateInfo);
		}
		mRenderSemaphores.assign(mSwapchainImages.size(), {});
		for (VkSemaphore& semaphore: mRenderSemaphores) {
			VK_CHECK(vkCreateSemaphore(mDevice, &semaphoreCreateInfo, nullptr, &semaphore));
		}

		VK_CHECK(vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &mImmediateFence));
	}

	void RenderContext::initDescriptorSets() {
		std::vector<DescriptorPoolSizeRatio> poolSizes = {
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1}
		};
		mDescriptorAllocator.init(mDevice, 10, poolSizes);

		DescriptorSetLayoutBuilder descriptorSetLayoutBuilder;
		descriptorSetLayoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		mDrawImageDescriptorSetLayout = descriptorSetLayoutBuilder.build(mDevice, VK_SHADER_STAGE_COMPUTE_BIT);

		mDrawImageDescriptorSet = mDescriptorAllocator.allocate(mDevice, mDrawImageDescriptorSetLayout);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageInfo.imageView = mDrawImage.imageView;

		VkWriteDescriptorSet writeDescriptorSet = {};
		writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSet.dstSet = mDrawImageDescriptorSet;
		writeDescriptorSet.descriptorCount = 1;
		writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		writeDescriptorSet.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(mDevice, 1, &writeDescriptorSet, 0, nullptr);
	}

	void RenderContext::destroySwapchain() const {
		vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);

		for (const VkImageView image: mSwapchainImageViews) {
			vkDestroyImageView(mDevice, image, nullptr);
		}
	}
}
