#pragma once

#include <vector>

#include "AllocatedImage.h"
#include "DescriptorAllocator.h"
#include "vk_mem_alloc.h"

#include "WindowManager.h"

namespace voxel_game::client::renderer {
	constexpr uint32_t FRAME_OVERLAP = 2;

	class RenderContext;

	class FrameContext {
	public:
		void initCommands(const RenderContext* context, const VkCommandPoolCreateInfo &commandPoolInfo);

		void initSyncStructures(const RenderContext* context, const VkFenceCreateInfo &fenceCreateInfo, const VkSemaphoreCreateInfo &semaphoreCreateInfo);

		[[nodiscard]] VkCommandPool getCommandPool() const {
			return mCommandPool;
		}

		[[nodiscard]] VkCommandBuffer getCommandBuffer() const {
			return mCommandBuffer;
		}

		[[nodiscard]] VkSemaphore getSwapchainSemaphore() const {
			return mSwapchainSemaphore;
		}

		[[nodiscard]] VkFence& getRenderFence() {
			return mRenderFence;
		}

		void destroy(const RenderContext* context) const;

	private:
		VkCommandPool mCommandPool = nullptr;
		VkCommandBuffer mCommandBuffer = nullptr;

		VkSemaphore mSwapchainSemaphore = nullptr;
		VkFence mRenderFence = nullptr;
	};

	class RenderContext {
	public:
		explicit RenderContext(const WindowManager* windowManager);

		[[nodiscard]] VkInstance getInstance() const {
			return mInstance;
		}

		[[nodiscard]] VkSurfaceKHR getSurface() const {
			return mSurface;
		}

		[[nodiscard]] VkPhysicalDevice getPhysicalDevice() const {
			return mPhysicalDevice;
		}

		[[nodiscard]] VkDevice getDevice() const {
			return mDevice;
		}

		[[nodiscard]] VkSwapchainKHR& getSwapchain() {
			return mSwapchain;
		}

		[[nodiscard]] VkFormat getSwapchainImageFormat() const {
			return mSwapchainImageFormat;
		}

		[[nodiscard]] std::vector<VkImage>& getSwapchainImages() {
			return mSwapchainImages;
		}

		[[nodiscard]] std::vector<VkImageView>& getSwapchainImageViews() {
			return mSwapchainImageViews;
		}

		[[nodiscard]] VkExtent2D getSwapchainExtent() const {
			return mSwapchainExtent;
		}

		[[nodiscard]] FrameContext getCurrentFrameContext() const {
			return mFrameContexts[mFrame % FRAME_OVERLAP];
		}

		[[nodiscard]] uint64_t getCurrentFrame() const {
			return mFrame;
		}

		[[nodiscard]] VkQueue getGraphicsQueue() const {
			return mGraphicsQueue;
		}

		[[nodiscard]] uint32_t getGraphicsQueueFamily() const {
			return mGraphicsQueueFamily;
		}

		[[nodiscard]] std::vector<VkSemaphore>& getRenderSemaphores() {
			return mRenderSemaphores;
		}

		[[nodiscard]] VmaAllocator getAllocator() const {
			return mAllocator;
		}

		[[nodiscard]] AllocatedImage& getDrawImage() {
			return mDrawImage;
		}

		[[nodiscard]] VkExtent2D& getDrawExtent() {
			return mDrawExtent;
		}

		[[nodiscard]] DescriptorAllocator& getDescriptorAllocator() {
			return mDescriptorAllocator;
		}

		[[nodiscard]] VkDescriptorSet& getDrawImageDescriptorSet() {
			return mDrawImageDescriptorSet;
		}

		[[nodiscard]] VkDescriptorSetLayout& getDrawImageDescriptorSetLayout() {
			return mDrawImageDescriptorSetLayout;
		}


		void nextFrame() {
			mFrame++;
		}

		void resizeSwapchain(const WindowManager* windowManager);

		void destroy() const;

	private:
		VkInstance mInstance = nullptr;
		VkDebugUtilsMessengerEXT mDebugMessenger = nullptr;
		VkSurfaceKHR mSurface = nullptr;
		VkPhysicalDevice mPhysicalDevice = nullptr;
		VkDevice mDevice = nullptr;

		VkSwapchainKHR mSwapchain = nullptr;
		VkFormat mSwapchainImageFormat = VK_FORMAT_UNDEFINED;
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;
		VkExtent2D mSwapchainExtent = {};

		FrameContext mFrameContexts[FRAME_OVERLAP];
		uint64_t mFrame = 0;

		VkQueue mGraphicsQueue = nullptr;
		uint32_t mGraphicsQueueFamily = 0;

		std::vector<VkSemaphore> mRenderSemaphores;

		VmaAllocator mAllocator = nullptr;

		AllocatedImage mDrawImage = {};
		VkExtent2D mDrawExtent = {};

		DescriptorAllocator mDescriptorAllocator;

		VkDescriptorSet mDrawImageDescriptorSet = nullptr;
		VkDescriptorSetLayout mDrawImageDescriptorSetLayout = nullptr;

		void initVulkan(const WindowManager* windowManager);

		void initSwapchain(uint32_t width, uint32_t height);

		void initCommands();

		void initSyncStructures();

		void initDescriptorSets();

		void destroySwapchain() const;
	};
}
