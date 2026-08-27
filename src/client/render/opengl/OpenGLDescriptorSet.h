#pragma once

#include "client/render/DescriptorSet.h"

namespace voxel_game::client::render::opengl {
	class OpenGLDescriptorSet : public DescriptorSet {
	public:
		void setBinding(uint32_t binding, GPUImage* image) override;

		void setBinding(uint32_t binding, GPUBuffer* buffer) override;

		void destroy();

		void bind(uint32_t set) const;

	private:
		std::vector<GPUImage*> mImageBindings;
		std::vector<GPUBuffer*> mBufferBindings;
	};
}
