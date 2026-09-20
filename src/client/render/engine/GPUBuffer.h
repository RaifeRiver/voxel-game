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

#include <cstdint>
#include <cstddef>

namespace voxel_game::client::render::engine {
    enum class BufferUsage : uint32_t {
        NONE = 0,
        TRANSFER_SRC = 1 << 0,
        TRANSFER_DST = 1 << 1,
        UNIFORM = 1 << 2,
        STORAGE = 1 << 3,
        SHADER_DEVICE_ADDRESS = 1 << 4,
    	INDEX = 1 << 5,
    	INDIRECT = 1 << 6
    };

    inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
		return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline uint32_t operator&(BufferUsage a, BufferUsage b) {
		return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
	}

    enum class MemoryType {
        AUTO,
        CPU,
        GPU
    };

    enum class MappedType {
        NONE,
        SEQUENTIAL_WRITE,
        RANDOM_ACCESS
    };

	enum class BufferAccess : uint32_t {
		NONE = 0,
		TRANSFER_READ = 1 << 0,
		TRANSFER_WRITE = 1 << 1,
		UNIFORM_READ = 1 << 2,
		SHADER_READ = 1 << 3,
		SHADER_WRITE = 1 << 4,
		INDEX_READ = 1 << 5,
		INDIRECT_READ = 1 << 6
	};

	inline BufferAccess operator|(BufferAccess a, BufferAccess b) {
		return static_cast<BufferAccess>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline uint32_t operator&(BufferAccess a, BufferAccess b) {
		return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
	}

    class GPUBuffer {
    public:
		GPUBuffer(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType);

    	void* map();

    	void unmap();

    	uint64_t getDeviceAddress();

    	void copyFromBuffer(GPUBuffer& other, uint32_t srcOffset, uint32_t dstOffset, uint32_t size);

    	void fill(uint32_t offset, uint32_t size, uint32_t value);

		void barrier(BufferAccess srcAccess, BufferAccess dstAccess, uint32_t offset = 0, uint32_t size = UINT32_MAX);

    	[[nodiscard]] size_t getSize() const {
    		return mSize;
    	}

    	[[nodiscard]] BufferUsage getUsage() const {
    		return mUsage;
    	}

    	[[nodiscard]] MemoryType getMemoryType() const {
    		return mMemoryType;
    	}

    	[[nodiscard]] MappedType getMappedType() const {
    		return mMappedType;
    	}

    	virtual ~GPUBuffer() = default;

    protected:
    	size_t mSize;
    	BufferUsage mUsage;
    	MemoryType mMemoryType;
    	MappedType mMappedType;
    	bool mMapped = false;

    	virtual void* map_() = 0;

    	virtual void unmap_() = 0;

    	virtual uint64_t getDeviceAddress_() = 0;

    	virtual void copyFromBuffer_(GPUBuffer& other, uint32_t srcOffset, uint32_t dstOffset, uint32_t size) = 0;

    	virtual void fill_(uint32_t offset, uint32_t size, uint32_t value) = 0;

		virtual void barrier_(BufferAccess srcAccess, BufferAccess dstAccess, uint32_t offset, uint32_t size) = 0;
    };
}
