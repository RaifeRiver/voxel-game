#pragma once

#include "client/render/engine/DescriptorSet.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLDescriptorAllocator;

	class OpenGLDescriptorSet : public DescriptorSet {
	public:
		explicit OpenGLDescriptorSet(OpenGLDescriptorAllocator* descriptorAllocator);

		void setBinding(uint32_t binding, GPUImage* image) override;

		void setBinding(uint32_t binding, GPUBuffer* buffer) override;

		void destroy();

		void bind(uint32_t set) const;

	private:
		OpenGLDescriptorAllocator* mDescriptorAllocator;
		std::vector<GPUImage*> mImageBindings;
		std::vector<GPUBuffer*> mBufferBindings;
	};
}
