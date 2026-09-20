/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include "volk.h"
#include "tracy/TracyVulkan.hpp"
#include "vk_mem_alloc.h"

#include "VulkanImage.h"
#include "client/render/engine/DescriptorAllocator.h"
#include "client/render/engine/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::engine::vulkan {
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

		std::unique_ptr<ComputePipelineBuilder> createComputePipelineBuilder(const Shader& computeShader) override;

		std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader) override;

		std::unique_ptr<DescriptorLayoutBuilder> createDescriptorLayoutBuilder() override;

		[[nodiscard]] GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

		[[nodiscard]] GPUImage& getDepthImage() override {
			return *mDepthImage;
		}

		void beginRendering() override;

		void endRendering() override;

		void waitForGPU() override;

		void submitImmediate(const std::function<void(VkCommandBuffer)>& function) const;

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

		[[nodiscard]] TracyVkCtx getTracyContext() const {
			return mTracyContext;
		}

		void destroy() override;

	protected:
		std::unique_ptr<GPUBuffer> allocateBuffer_(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType) override;

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
		uint32_t mCurrentSwapchainIndex = 0;
		VulkanFrameData mFrameData[FRAME_OVERLAP] = {};
		std::vector<VkSemaphore> mRenderSemaphores;
		std::unique_ptr<VulkanImage> mRenderImage;
		std::unique_ptr<VulkanImage> mDepthImage;
		VkCommandPool mImmediateCommandPool = nullptr;
		VkCommandBuffer mImmediateCommandBuffer = nullptr;
		VkFence mImmediateFence = nullptr;
		TracyVkCtx mTracyContext = nullptr;
		bool mRendering = false;
		bool mNeedsResize = false;

		VulkanFrameData& getFrameData() {
			return mFrameData[mFrame % FRAME_OVERLAP];
		}

		void createInstance(window::Window& window);

		void selectPhysicalDevice(std::optional<uint32_t> gpu);

		void createDevice(window::Window& window);

		void createAllocator();

		void createSurface(window::Window& window);

		void createSwapchain(window::Window& window, bool vsync);

		void createCommandBuffers();

		void createSyncStructures();

		void initTracyContext();

		void resizeSwapchain(window::Window& window, bool vsync);

		void preRender(window::Window& window, bool vsync);

		void postRender();

		void destroySwapchain();

		static bool checkValidationLayerSupport();
	};
}
