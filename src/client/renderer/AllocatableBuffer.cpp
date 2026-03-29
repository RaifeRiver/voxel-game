#include "AllocatableBuffer.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace voxel_game::client::renderer {
	AllocatableBuffer::AllocatableBuffer(const RenderContext *renderContext, const uint32_t size, const VkBufferUsageFlags usage, const VmaMemoryUsage memoryUsage, const VmaAllocationCreateFlags flags) : mSize(std::bit_ceil(size)), mFree(size) {
		mBuffer = createBuffer(renderContext, mSize, usage, memoryUsage, flags);
		uint32_t maxOrder = std::bit_width(mSize);
		mFreeMemory->resize(maxOrder + 1);
		mFreeMemory[maxOrder - 1].push_back(0);
	}

	Allocation AllocatableBuffer::allocate(uint32_t size) {
		size = std::bit_ceil(size);
		uint32_t currentSize = size;
		uint32_t index = std::countr_zero(currentSize);
		std::vector<uint32_t> &freeList = mFreeMemory[index];
		if (!freeList.empty()) {
			uint32_t offset = freeList.back();
			freeList.pop_back();
			mFree -= size;
			return {offset, size};
		}
		if (mFree - size < 0) {
			throw std::runtime_error("AllocatableBuffer is full");
		}
		uint32_t searchIndex = index;
		while (searchIndex < mFreeMemory->size() && mFreeMemory[searchIndex].empty()) {
			searchIndex++;
		}

		if (searchIndex >= mFreeMemory->size()) {
			throw std::runtime_error("AllocatableBuffer is fragmented");
		}

		uint32_t offset = mFreeMemory[searchIndex].back();
		mFreeMemory[searchIndex].pop_back();

		while (searchIndex > index) {
			searchIndex--;
			uint32_t halfSize = 1U << searchIndex;
			mFreeMemory[searchIndex].push_back(offset + halfSize);
		}

		mFree -= size;
		return {offset, size};
	}

	void AllocatableBuffer::free(const Allocation allocation) {
		uint32_t size = allocation.size;
		uint32_t offset = allocation.offset;
		uint32_t index = std::countr_zero(size);
		while (size <= mSize) {
			uint32_t buddyOffset = offset ^ size;
			std::vector<uint32_t> &freeList = mFreeMemory[index];
			auto it = std::ranges::find(freeList, buddyOffset);
			if (it != freeList.end()) {
				freeList.erase(it);
				offset = std::min(offset, buddyOffset);
				size <<= 1;
				index++;
			}
			else {
				break;
			}
		}
		mFreeMemory[index].push_back(offset);
		mFree += allocation.size;
	}

	void AllocatableBuffer::destroy(const RenderContext *renderContext) const {
		destroyBuffer(renderContext, mBuffer);
	}
}
