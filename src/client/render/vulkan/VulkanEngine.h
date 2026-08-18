#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "volk.h"
#include "vk_mem_alloc.h"

#include "VulkanImage.h"
#include "client/render/DescriptorAllocator.h"
#include "client/render/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::vulkan {
#ifdef VG_DEBUG
	constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
	constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

	struct VulkanFrameData {
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;

		VkSemaphore swapchainSemaphore;
		VkFence renderFence;
	};

	class VulkanEngine : public RenderEngine {
	public:
		explicit VulkanEngine(ecs::ECSRegistry& registry);

		std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) override;

		std::unique_ptr<ComputePipeline> createComputePipeline(const std::string& computeShader) override;

		std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() override;

		[[nodiscard]] GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

		void waitForGPU() override;

		[[nodiscard]] VmaAllocator getVMAAllocator() const {
			return mAllocator;
		}

		[[nodiscard]] VkDevice getDevice() const {
			return mDevice;
		}

		[[nodiscard]] VkCommandBuffer getCommandBuffer() {
			if (!mRendering) {
				throw std::runtime_error("Must be called between preRender() and postRender()");
			}
			return getFrameData().commandBuffer;
		}

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
		glm::uvec3 mSwapchainExtent = {};
		VulkanFrameData mFrameData[FRAME_OVERLAP] = {};
		std::vector<VkSemaphore> mRenderSemaphores;
		std::unique_ptr<VulkanImage> mRenderImage;
		uint32_t mCurrentSwapchainIndex = 0;
		bool mRendering = false;
		bool mNeedsResize = false;

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

		void resizeSwapchain(window::Window& window);

		void preRender(window::Window& window);

		void postRender();

		void destroySwapchain();

		static bool checkValidationLayerSupport();
	};
}
