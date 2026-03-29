#pragma once

#include <vulkan/vulkan.h>

#include "RenderContext.h"
#include "vk_mem_alloc.h"

namespace voxel_game::client::renderer {
	struct AllocatedBuffer {
		VkBuffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
	};

	AllocatedBuffer createBuffer(const RenderContext *renderContext, size_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags);

	void destroyBuffer(const RenderContext *renderContext, const AllocatedBuffer& buffer);
}
