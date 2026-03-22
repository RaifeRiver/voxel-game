#include "RenderContext.h"

#include "VkBootstrap.h"
#define VMA_IMPLEMENTATION
#include "DescriptorSetLayoutBuilder.h"
#include "vk_mem_alloc.h"
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
		initDescriptorSets();
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

		for (const VkSemaphore& semaphore: mRenderSemaphores) {
			vkDestroySemaphore(mDevice, semaphore, nullptr);
		}

		mDescriptorAllocator.destroy(mDevice);

		vkDestroyDescriptorSetLayout(mDevice, mDrawImageDescriptorSetLayout, nullptr);

		vkDestroyImageView(mDevice, mDrawImage.imageView, nullptr);
		vmaDestroyImage(mAllocator, mDrawImage.image, mDrawImage.allocation);

		vmaDestroyAllocator(mAllocator);

		destroySwapchain();

		vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
		vkDestroyDevice(mDevice, nullptr);

		vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
		vkDestroyInstance(mInstance, nullptr);
	}

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

		vkb::PhysicalDeviceSelector physicalDeviceSelector{instance, mSurface};
		vkb::PhysicalDevice physicalDevice = physicalDeviceSelector.set_minimum_version(1, 3).set_required_features_13(features13).set_required_features_12(features12).select().value();
		mPhysicalDevice = physicalDevice.physical_device;

		vkb::DeviceBuilder deviceBuilder{physicalDevice};
		vkb::Device device = deviceBuilder.build().value();
		mDevice = device.device;

		mGraphicsQueue = device.get_queue(vkb::QueueType::graphics).value();
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
		vkb::Swapchain swapchain = swapchainBuilder.set_desired_format(VkSurfaceFormatKHR{mSwapchainImageFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR).set_desired_extent(width, height).add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT).build().value();
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
