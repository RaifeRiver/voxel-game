#pragma once

#include "GPUBuffer.h"
#include "GPUImage.h"

namespace voxel_game::client::render {
	class DescriptorSet {
	public:
		virtual void setBinding(uint32_t binding, GPUImage* image) = 0;

		virtual void setBinding(uint32_t binding, GPUBuffer* buffer) = 0;

		virtual ~DescriptorSet() = default;
	};
}
