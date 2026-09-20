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
		VkBufferUsageFlags bufferUsage = 0;
		if (usage & BufferUsage::TRANSFER_SRC) {
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		}
		if (usage & BufferUsage::TRANSFER_DST) {
			bufferUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		if (usage & BufferUsage::UNIFORM) {
			bufferUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (usage & BufferUsage::STORAGE) {
			bufferUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (usage & BufferUsage::SHADER_DEVICE_ADDRESS) {
			bufferUsage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		}
		if (usage & BufferUsage::INDEX) {
			bufferUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (usage & BufferUsage::INDIRECT) {
			bufferUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		return bufferUsage;
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

	VkAccessFlags toVKAccessFlags(const BufferAccess access) {
		VkAccessFlags flags = 0;
		if (access & BufferAccess::TRANSFER_READ) {
			flags |= VK_ACCESS_TRANSFER_READ_BIT;
		}
		if (access & BufferAccess::TRANSFER_WRITE) {
			flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		if (access & BufferAccess::UNIFORM_READ) {
			flags |= VK_ACCESS_UNIFORM_READ_BIT;
		}
		if (access & BufferAccess::SHADER_READ) {
			flags |= VK_ACCESS_SHADER_READ_BIT;
		}
		if (access & BufferAccess::SHADER_WRITE) {
			flags |= VK_ACCESS_SHADER_WRITE_BIT;
		}
		if (access & BufferAccess::INDEX_READ) {
			flags |= VK_ACCESS_INDEX_READ_BIT;
		}
		if (access & BufferAccess::INDIRECT_READ) {
			flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		}
		return flags;
	}

	VkPipelineStageFlags toVkPipelineStageFlags(BufferAccess access) {
		VkPipelineStageFlags flags = 0;
		if (access & BufferAccess::TRANSFER_READ || access & BufferAccess::TRANSFER_WRITE) {
			flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		if (access & BufferAccess::UNIFORM_READ || access & BufferAccess::SHADER_READ || access & BufferAccess::SHADER_WRITE) {
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		if (access & BufferAccess::INDEX_READ) {
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		}
		if (access & BufferAccess::INDIRECT_READ) {
			flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		}
		return flags;
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

	void VulkanBuffer::copyFromBuffer_(GPUBuffer& other, const uint32_t srcOffset, const uint32_t dstOffset, const uint32_t size) {
		const VkBufferCopy bufferCopy = {
			.srcOffset = srcOffset,
			.dstOffset = dstOffset,
			.size = size
		};
		vkCmdCopyBuffer(mVulkanEngine->getCommandBuffer(), reinterpret_cast<VulkanBuffer&>(other).mBuffer, mBuffer, 1, &bufferCopy);
	}

	void VulkanBuffer::fill_(const uint32_t offset, const uint32_t size, const uint32_t value) {
		vkCmdFillBuffer(mVulkanEngine->getCommandBuffer(), mBuffer, offset, size, value);
	}

	void VulkanBuffer::barrier_(const BufferAccess srcAccess, const BufferAccess dstAccess, const uint32_t offset, const uint32_t size) {
		const VkBufferMemoryBarrier memoryBarrier = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			.srcAccessMask = toVKAccessFlags(srcAccess),
			.dstAccessMask = toVKAccessFlags(dstAccess),
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.buffer = mBuffer,
			.offset = offset,
			.size = size == UINT32_MAX ? VK_WHOLE_SIZE : size
		};
		vkCmdPipelineBarrier(mVulkanEngine->getCommandBuffer(), toVkPipelineStageFlags(srcAccess), toVkPipelineStageFlags(dstAccess), 0, 0, nullptr, 1, &memoryBarrier, 0, nullptr);
	}
}
