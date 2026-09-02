#include "DescriptorAllocator.h"

namespace voxel_game::client::render::engine {
	DescriptorAllocatorBuilder* DescriptorAllocatorBuilder::addBinding(uint32_t binding, DescriptorType type) {
		mBindings.emplace_back(binding, type);
		return this;
	}
}
