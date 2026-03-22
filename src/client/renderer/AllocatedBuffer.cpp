#include "AllocatedBuffer.h"

#include "VulkanUtil.h"

namespace voxel_game::client::renderer {
	AllocatedBuffer createBuffer(const RenderContext* renderContext, const size_t size, const VkBufferUsageFlags usage, const VmaMemoryUsage memoryUsage) {
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;

		VmaAllocationCreateInfo allocationInfo = {};
		allocationInfo.usage = memoryUsage;
		allocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		AllocatedBuffer buffer;
		VK_CHECK(vmaCreateBuffer(renderContext->getAllocator(), &bufferInfo, &allocationInfo, &buffer.buffer, &buffer.allocation, &buffer.allocationInfo));
		return buffer;
	}

	void destroyBuffer(const RenderContext* renderContext, const AllocatedBuffer& buffer) {
		vmaDestroyBuffer(renderContext->getAllocator(), buffer.buffer, buffer.allocation);
	}
}
