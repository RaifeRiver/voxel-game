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

#include "VulkanBuffer.h"

#include <stdexcept>

#include "vk_mem_alloc.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::engine::vulkan {
	VkBufferUsageFlags toVKBufferUsage(const BufferUsage usage) {
		switch (usage) {
			case BufferUsage::NONE:
				return 0;
			case BufferUsage::TRANSFER_SRC:
				return VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			case BufferUsage::TRANSFER_DST:
				return VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			case BufferUsage::UNIFORM:
				return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			case BufferUsage::STORAGE:
				return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			case BufferUsage::SHADER_DEVICE_ADDRESS:
				return VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
			case BufferUsage::INDEX:
				return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			default:
				throw std::runtime_error("Unsupported buffer usage");
		}
	}

	VmaMemoryUsage toVMAMemoryUsage(const MemoryType memoryType) {
		switch (memoryType) {
			case MemoryType::AUTO:
				return VMA_MEMORY_USAGE_AUTO;
			case MemoryType::CPU:
				return VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
			case MemoryType::GPU:
				return VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
			default:
				throw std::runtime_error("Unsupported memory type");
		}
	}

	VmaAllocationCreateFlags toVMAAllocationFlags(const MappedType mappedType) {
		switch (mappedType) {
			case MappedType::NONE:
				return 0;
			case MappedType::SEQUENTIAL_WRITE:
				return VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
			case MappedType::RANDOM_ACCESS:
				return VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
			default:
				throw std::runtime_error("Unsupported mapped type");
		}
	}

	VulkanBuffer::VulkanBuffer(VulkanEngine* vulkanEngine, const size_t size, const BufferUsage usage, const MemoryType memoryType, const MappedType mappedType) : GPUBuffer(size, usage, memoryType, mappedType), mVulkanEngine(vulkanEngine) {
		const VkBufferCreateInfo bufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = size,
			.usage = toVKBufferUsage(usage),
		};
		const VmaAllocationCreateInfo allocationCreateInfo = {
			.flags = toVMAAllocationFlags(mappedType),
			.usage = toVMAMemoryUsage(memoryType)
		};
		vulkan_util::vkCheck(vmaCreateBuffer(vulkanEngine->getVMAAllocator(), &bufferCreateInfo, &allocationCreateInfo, &mBuffer, &mAllocation, &mAllocationInfo));
	}

	VulkanBuffer::~VulkanBuffer() {
		vmaDestroyBuffer(mVulkanEngine->getVMAAllocator(), mBuffer, mAllocation);
	}

	void* VulkanBuffer::map_() {
		return mAllocationInfo.pMappedData;
	}

	void VulkanBuffer::unmap_() {}

	uint64_t VulkanBuffer::getDeviceAddress_() {
		const VkBufferDeviceAddressInfo bufferDeviceAddressInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
			.buffer = mBuffer
		};
		return vkGetBufferDeviceAddress(mVulkanEngine->getDevice(), &bufferDeviceAddressInfo);
	}
}
