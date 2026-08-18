#pragma once

#include "client/render/GPUImage.h"

namespace voxel_game::client::render::opengl {
	class OpenGLRenderImage : public GPUImage {
	public:
		explicit OpenGLRenderImage(const glm::uvec3 &size);

		void transition(ImageUsage usage) override;

		void clearColour(glm::vec4 colour) override;
	};
}
