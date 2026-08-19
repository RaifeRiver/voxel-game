#include "OpenGLDescriptorAllocator.h"

#include "OpenGLDescriptorSet.h"

namespace voxel_game::client::render::opengl {
	OpenGLDescriptorAllocator::OpenGLDescriptorAllocator(const uint32_t maxSets) : mMaxSets(maxSets) {}

	void OpenGLDescriptorAllocator::clearDescriptors() {
		for (OpenGLDescriptorSet* descriptorSet: mDescriptorSets) {
			descriptorSet->destroy();
		}
		mDescriptorSets.clear();
	}

	std::unique_ptr<DescriptorSet> OpenGLDescriptorAllocator::allocate() {
		if (mDescriptorSets.size() == mMaxSets) {
			throw std::runtime_error("Too many descriptor sets");
		}
		auto descriptorSet = std::make_unique<OpenGLDescriptorSet>();
		mDescriptorSets.push_back(descriptorSet.get());
		return descriptorSet;
	}

	std::unique_ptr<DescriptorAllocator> OpenGLDescriptorAllocatorBuilder::build(uint32_t maxSets, ShaderStage shaderStages) {
		return std::make_unique<OpenGLDescriptorAllocator>(maxSets);
	}
}
