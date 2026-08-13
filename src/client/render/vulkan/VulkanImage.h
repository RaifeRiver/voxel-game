#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

#include "client/render/GPUImage.h"

namespace voxel_game::client::render::vulkan {
	class VulkanEngine;

	VkFormat toVKImageFormat(ImageFormat format);

	VkImageUsageFlagBits toVKImageUsage(ImageUsage usage);

	VkImageType toVKImageType(ImageType type);

	VkImageViewType toVKImageViewType(ImageType type);

	VkImageAspectFlags toVKImageAspectFlags(ImageFormat format);

	class VulkanImage : public GPUImage {
	public:
		VulkanImage(const VulkanEngine* vulkanEngine, glm::uvec3 size, ImageFormat format, ImageUsage usage, ImageType type);

	private:
		VkImage mImage = nullptr;
		VkImageView mImageView = nullptr;
		VmaAllocation mAllocation = nullptr;
	};
}
