#pragma once

#include "OpenGLDescriptorSet.h"
#include "client/render/DescriptorAllocator.h"

namespace voxel_game::client::render::opengl {
	class OpenGLDescriptorAllocator : public DescriptorAllocator {
	public:
		explicit OpenGLDescriptorAllocator(uint32_t maxSets);

		void clearDescriptors() override;

		std::unique_ptr<DescriptorSet> allocate() override;

	private:
		uint32_t mMaxSets;
		std::vector<OpenGLDescriptorSet*> mDescriptorSets = {};
	};

	class OpenGLDescriptorAllocatorBuilder : public DescriptorAllocatorBuilder {
	public:
		std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) override;
	};
}
