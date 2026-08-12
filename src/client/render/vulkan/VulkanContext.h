#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

#include "client/render/RenderContext.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::vulkan {
#ifdef VG_DEBUG
	constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
	constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

	constexpr uint32_t FRAME_OVERLAP = 2;

	struct VulkanFrameData {
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;

		VkSemaphore swapchainSemaphore;
		VkFence renderFence;
	};

	class VulkanContext : public RenderContext {
	public:
		explicit VulkanContext(ecs::ECSRegistry& registry);

		void destroy() override;

	private:
		VkInstance mInstance = nullptr;
		VkPhysicalDevice mPhysicalDevice = nullptr;
		VkDevice mDevice = nullptr;
		VkQueue mGraphicsQueue = nullptr;
		uint32_t mGraphicsQueueFamily = 0;
		VmaAllocator mAllocator = nullptr;
		VkSurfaceKHR mSurface = nullptr;
		VkSwapchainKHR mSwapchain = nullptr;
		std::vector<VkImage> mSwapchainImages;
		std::vector<VkImageView> mSwapchainImageViews;
		VkImage mDepthImage = nullptr;
		VmaAllocation mDepthImageAllocation = nullptr;
		VkImageView mDepthImageView = nullptr;
		VulkanFrameData mFrameData[FRAME_OVERLAP] = {};
		std::vector<VkSemaphore> mRenderSemaphores;
		uint32_t mFrame = 0;

		void createInstance(window::Window& window);

		void selectPhysicalDevice();

		void createDevice(window::Window& window);

		void createAllocator();

		void createSurface(window::Window& window);

		void createSwapchain(window::Window& window);

		void createCommandBuffers();

		void createSyncStructures();

		static bool checkValidationLayerSupport();
	};
}
