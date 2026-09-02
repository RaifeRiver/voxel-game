#pragma once

#include <string>

#include "OpenGLUtil.h"
#include "client/render/engine/ComputePipeline.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLComputePipeline : public ComputePipeline {
	public:
		explicit OpenGLComputePipeline(const std::string& computeShaderGLSL);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		void setPushConstants(void* pushConstants) override;

		~OpenGLComputePipeline() override;

	protected:
		void dispatch_(uint32_t x, uint32_t y, uint32_t z) override;

	private:
		unsigned int mShaderProgram;
		std::vector<opengl_util::PushConstant> mPushConstants;
	};
}
