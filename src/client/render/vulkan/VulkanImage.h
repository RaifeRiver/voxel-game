#pragma once

#include "volk.h"
#include "vk_mem_alloc.h"

#include "client/render/GPUImage.h"

namespace voxel_game::client::render::vulkan {
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

		void transition(ImageUsage usage);

		void clearColour(glm::vec4 colour) override;

		~VulkanImage() override;

		[[nodiscard]] VkImage getImage() const {
			return mImage;
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
