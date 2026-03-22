#pragma once

#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

namespace voxel_game::client::renderer {
	struct AllocatedImage {
		VkImage image;
		VkImageView imageView;
		VmaAllocation allocation;
		VkExtent3D extent;
		VkFormat format;
	};
}