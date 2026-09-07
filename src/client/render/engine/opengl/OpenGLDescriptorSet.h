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
