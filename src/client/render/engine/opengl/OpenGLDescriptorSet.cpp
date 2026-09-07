/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "OpenGLDescriptorSet.h"

#include "glad/glad.h"

#include "OpenGLBuffer.h"
#include "OpenGLDescriptorAllocator.h"
#include "OpenGLImage.h"
#include "client/render/engine/DescriptorAllocator.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLDescriptorSet::OpenGLDescriptorSet(OpenGLDescriptorAllocator* descriptorAllocator) : mDescriptorAllocator(descriptorAllocator) {}

	void OpenGLDescriptorSet::setBinding(const uint32_t binding, GPUImage* image) {
		if (mImageBindings.size() <= binding) {
			mImageBindings.resize(binding + 1);
		}
		mImageBindings[binding] = image;
	}

	void OpenGLDescriptorSet::setBinding(const uint32_t binding, GPUBuffer* buffer) {
		if (mBufferBindings.size() <= binding) {
			mBufferBindings.resize(binding + 1);
		}
		mBufferBindings[binding] = buffer;
	}

	void OpenGLDescriptorSet::destroy() {
		mImageBindings.clear();
		mBufferBindings.clear();
	}

	void OpenGLDescriptorSet::bind(const uint32_t set) const {
		const std::vector<DescriptorBinding>& descriptorBindings = mDescriptorAllocator->getDescriptorBindings();
		for (uint32_t i = 0; i < mImageBindings.size(); i++) {
			if (mImageBindings[i]) {
				for (const auto &[binding, type] : descriptorBindings) {
					if (binding == i) {
						const OpenGLImage* image = dynamic_cast<OpenGLImage*>(mImageBindings[i]);
						if (type == DescriptorType::SAMPLED_TEXTURE || type == DescriptorType::TEXTURE) {
							glBindTexture((set << 4) | i, image->getImage());
						}
						else if (type == DescriptorType::IMAGE) {
							glBindImageTexture((set << 4) | i, image->getImage(), 0, false, 0, GL_READ_WRITE, toOpenGLImageFormat(image->getFormat()));
						}
						else {
							throw std::runtime_error("Can not bind image to non image descriptor");
						}
						return;
					}
				}
				throw std::runtime_error("No binding " + std::to_string(i));
			}
		}
		for (uint32_t i = 0; i < mBufferBindings.size(); i++) {
			if (mBufferBindings[i]) {
				for (const auto &[binding, type] : descriptorBindings) {
					if (binding == i) {
						const OpenGLBuffer* buffer = dynamic_cast<OpenGLBuffer*>(mBufferBindings[i]);
						if (type == DescriptorType::UNIFORM_BUFFER) {
							glBindBufferBase(GL_UNIFORM_BUFFER, (set << 4) | i, buffer->getBuffer());
						}
						else if (type == DescriptorType::STORAGE_BUFFER) {
							glBindBufferBase(GL_SHADER_STORAGE_BUFFER, (set << 4) | i, buffer->getBuffer());
						}
						else {
							throw std::runtime_error("Can not bind buffer to non buffer descriptor");
						}
						return;
					}
				}
				throw std::runtime_error("No binding " + std::to_string(i));
			}
		}
	}
}
