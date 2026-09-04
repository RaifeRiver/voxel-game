#pragma once

#include "volk.h"
#include "vk_mem_alloc.h"

#include "client/render/engine/GPUBuffer.h"

namespace voxel_game::client::render::engine::vulkan {
	class VulkanEngine;

	VkBufferUsageFlags toVKBufferUsage(BufferUsage usage);

	VmaMemoryUsage toVMAMemoryUsage(MemoryType memoryType);

	VmaAllocationCreateFlags toVMAAllocationFlags(MappedType mappedType);

	class VulkanBuffer : public GPUBuffer {
	public:
		VulkanBuffer(VulkanEngine* vulkanEngine, size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType);

		[[nodiscard]] VkBuffer getBuffer() const {
			return mBuffer;
		}

		~VulkanBuffer() override;

	protected:
		void* map_() override;

		void unmap_() override;

		uint64_t getDeviceAddress_() override;

	private:
		VulkanEngine* mVulkanEngine;
		VkBuffer mBuffer = nullptr;
		VmaAllocation mAllocation = nullptr;
		VmaAllocationInfo mAllocationInfo = {};
	};
}
