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
    	INDEX = 1 << 5
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

    class GPUBuffer {
    public:
		GPUBuffer(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType);

    	void* map();

    	void unmap();

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
    };
}
