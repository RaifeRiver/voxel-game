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
