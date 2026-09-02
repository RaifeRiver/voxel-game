#include "OpenGLDescriptorAllocator.h"

#include "OpenGLDescriptorSet.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLDescriptorAllocator::OpenGLDescriptorAllocator(const uint32_t maxSets, const std::vector<DescriptorBinding>& descriptorBindings) : mMaxSets(maxSets), mDescriptorBindings(descriptorBindings) {}

	void OpenGLDescriptorAllocator::clearDescriptors() {
		for (OpenGLDescriptorSet* descriptorSet: mDescriptorSets) {
			descriptorSet->destroy();
		}
		mDescriptorSets.clear();
	}

	std::unique_ptr<DescriptorSet> OpenGLDescriptorAllocator::allocate() {
		if (mDescriptorSets.size() == mMaxSets) {
			throw std::runtime_error("Too many descriptor sets allocated");
		}
		auto descriptorSet = std::make_unique<OpenGLDescriptorSet>(this);
		mDescriptorSets.push_back(descriptorSet.get());
		return descriptorSet;
	}

	std::unique_ptr<DescriptorAllocator> OpenGLDescriptorAllocatorBuilder::build(uint32_t maxSets, ShaderStage) {
		return std::make_unique<OpenGLDescriptorAllocator>(maxSets, mBindings);
	}
}
