#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

#include "client/render/RenderEngine.h"
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

	class VulkanEngine : public RenderEngine {
	public:
		explicit VulkanEngine(ecs::ECSRegistry& registry);

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
		VulkanFrameData mFrameData[FRAME_OVERLAP] = {};
		std::vector<VkSemaphore> mRenderSemaphores;
		uint64_t mFrame = 0;
		uint32_t mCurrentSwapchainIndex = 0;

		VulkanFrameData& getFrameData() {
			return mFrameData[mFrame % FRAME_OVERLAP];
		}

		void createInstance(window::Window& window);

		void selectPhysicalDevice();

		void createDevice(window::Window& window);

		void createAllocator();

		void createSurface(window::Window& window);

		void createSwapchain(window::Window& window);

		void createCommandBuffers();

		void createSyncStructures();

		void preRender();

		void postRender();

		static bool checkValidationLayerSupport();
	};
}
