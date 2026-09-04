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
}
