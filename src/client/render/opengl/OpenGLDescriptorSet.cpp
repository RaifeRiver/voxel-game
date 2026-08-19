#include "OpenGLDescriptorSet.h"

#include "OpenGLImage.h"
#include "glad/glad.h"

namespace voxel_game::client::render::opengl {
	void OpenGLDescriptorSet::setBinding(const uint32_t binding, GPUImage* image) {
		if (mImageBindings.size() <= binding) {
			mImageBindings.resize(binding + 1);
		}
		mImageBindings[binding] = image;
	}

	void OpenGLDescriptorSet::destroy() {
		mImageBindings.clear();
	}

	void OpenGLDescriptorSet::bind(const uint32_t set) const {
		for (uint32_t i = 0; i < mImageBindings.size(); i++) {
			if (mImageBindings[i]) {
				const OpenGLImage* image = dynamic_cast<OpenGLImage*>(mImageBindings[i]);
				glBindImageTexture((set << 4) | i, image->getImage(), 0, false, 0, GL_READ_WRITE, toOpenGLImageFormat(image->getFormat()));
			}
		}
	}
}
