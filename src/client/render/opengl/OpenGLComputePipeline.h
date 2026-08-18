#pragma once

#include <string>

#include "client/render/ComputePipeline.h"

namespace voxel_game::client::render::opengl {
	class OpenGLComputePipeline : public ComputePipeline {
	public:
		explicit OpenGLComputePipeline(const std::string& computeShaderPath);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		~OpenGLComputePipeline() override;

	protected:
		void dispatch_(uint32_t x, uint32_t y, uint32_t z) override;

	private:
		unsigned int mShaderProgram;
	};
}
