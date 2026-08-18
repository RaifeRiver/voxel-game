#pragma once

#include <string>

#include "client/render/ComputePipeline.h"

namespace voxel_game::client::render::opengl {
	class OpenGLComputePipeline : public ComputePipeline {
	public:
		explicit OpenGLComputePipeline(const std::string& computeShaderPath);

		~OpenGLComputePipeline() override;

	private:
		unsigned int mShaderProgram;
	};
}
