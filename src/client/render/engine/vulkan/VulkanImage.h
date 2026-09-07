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

#include "volk.h"
#include "vk_mem_alloc.h"

#include "client/render/engine/GPUImage.h"

namespace voxel_game::client::render::engine::vulkan {
	class VulkanEngine;

	VkFormat toVKImageFormat(ImageFormat format);

	VkImageUsageFlagBits toVKImageUsage(ImageUsage usage);

	VkImageLayout toVKImageLayout(ImageUsage usage);

	VkImageType toVKImageType(ImageType type);

	VkImageViewType toVKImageViewType(ImageType type);

	VkImageAspectFlags toVKImageAspectFlags(ImageFormat format);

	class VulkanImage : public GPUImage {
	public:
		VulkanImage(VulkanEngine* vulkanEngine, glm::uvec3 size, ImageFormat format, ImageUsage usage, ImageType type);

		void transition(ImageUsage usage) override;

		void clearColour(glm::vec4 colour) override;

		~VulkanImage() override;

		[[nodiscard]] VkImage getImage() const {
			return mImage;
		}

		[[nodiscard]] VkImageLayout getCurrentLayout() const {
			return mCurrentLayout;
		}

		[[nodiscard]] VkImageView getImageView() const {
			return mImageView;
		}

	private:
		VkImage mImage = nullptr;
		VkImageView mImageView = nullptr;
		VmaAllocation mAllocation = nullptr;
		ImageUsage mCurrentUsage = ImageUsage::NONE;
		VkImageLayout mCurrentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VulkanEngine* mVulkanEngine = nullptr;
	};

	void transitionImage(VkCommandBuffer commandBuffer, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);

	void copyImage(VkCommandBuffer commandBuffer, VkImage src, VkImage dst, glm::uvec3 srcSize, glm::uvec3 dstSize);
}
