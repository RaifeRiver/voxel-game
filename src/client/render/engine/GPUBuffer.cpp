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

#include "GPUBuffer.h"

#include <stdexcept>

namespace voxel_game::client::render::engine {
	GPUBuffer::GPUBuffer(const size_t size, const BufferUsage usage, const MemoryType memoryType, const MappedType mappedType) : mSize(size), mUsage(usage), mMemoryType(memoryType), mMappedType(mappedType) {}

	void* GPUBuffer::map() {
		if (mMappedType == MappedType::NONE) {
			throw std::runtime_error("Buffers created with MappedType::NONE can not be mapped");
		}
		if (mMapped) {
			throw std::runtime_error("Buffer already mapped");
		}
		mMapped = true;
		return map_();
	}

	void GPUBuffer::unmap() {
		if (!mMapped) {
			throw std::runtime_error("Buffer not mapped");
		}
		mMapped = false;
		unmap_();
	}

	uint64_t GPUBuffer::getDeviceAddress() {
		if (!(mUsage & BufferUsage::SHADER_DEVICE_ADDRESS)) {
			throw std::runtime_error("Buffers created without BufferUsage::SHADER_DEVICE_ADDRESS do not have a readable device address");
		}
		return getDeviceAddress_();
	}

	void GPUBuffer::copyFromBuffer(GPUBuffer& other, const uint32_t srcOffset, const uint32_t dstOffset, const uint32_t size) {
		if (!(mUsage & BufferUsage::TRANSFER_DST)) {
			throw std::runtime_error("Can't copy into buffer created without BufferUsage::TRANSFER_DST");
		}
		if (!(other.mUsage & BufferUsage::TRANSFER_SRC)) {
			throw std::runtime_error("Can't copy from buffer created without BufferUsage::TRANSFER_SRC");
		}
		copyFromBuffer_(other, srcOffset, dstOffset, size);
	}

	void GPUBuffer::fill(const uint32_t offset, const uint32_t size, const uint32_t value) {
		if (!(mUsage & BufferUsage::TRANSFER_DST)) {
			throw std::runtime_error("Can't copy into buffer created without BufferUsage::TRANSFER_DST");
		}
		fill_(offset, size, value);
	}

	void GPUBuffer::barrier(const BufferAccess srcAccess, const BufferAccess dstAccess, const uint32_t offset, const uint32_t size) {
		if (size + offset > getSize() && size != UINT32_MAX) {
			throw std::runtime_error("Buffer barrier size exceeds buffer size");
		}
		barrier_(srcAccess, dstAccess, offset, size);
	}
}
