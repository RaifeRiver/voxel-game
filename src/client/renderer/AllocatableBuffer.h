#pragma once

#include "AllocatedBuffer.h"

namespace voxel_game::client::renderer {
	struct Allocation {
		uint32_t offset;
		uint32_t size;
	};

	class AllocatableBuffer {
	public:
		AllocatableBuffer(const RenderContext* renderContext, uint32_t size, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT);

		Allocation allocate(uint32_t size);

		void free(Allocation allocation);

		[[nodiscard]] AllocatedBuffer& getBuffer() {
			return mBuffer;
		}

		void destroy(const RenderContext *renderContext) const;

	private:
		AllocatedBuffer mBuffer{};
		std::vector<uint32_t> mFreeMemory[32];
		uint32_t mSize;
		uint32_t mFree;
	};
}
