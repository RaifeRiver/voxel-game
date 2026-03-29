#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace voxel_game::client::renderer {
	class RenderContext;

	struct AllocatedImage {
		VkImage image;
		VkImageView imageView;
		VmaAllocation allocation;
		VkExtent3D extent;
		VkFormat format;
	};

	AllocatedImage createImage(const RenderContext *renderContext, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY, VmaAllocationCreateFlags flags = 0, bool mipmapped = false);

	void destroyImage(const RenderContext *renderContext, const AllocatedImage &image);
}