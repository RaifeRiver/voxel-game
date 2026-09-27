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

#include "glad/glad.h"

#include "client/render/engine/GPUBuffer.h"

namespace voxel_game::client::render::engine::opengl {
	GLbitfield toOpenGLBufferAccess(BufferAccess srcAccess, BufferAccess dstAccess);

	class OpenGLBuffer : public GPUBuffer {
	public:
		OpenGLBuffer(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType);

		[[nodiscard]] unsigned int getBuffer() const {
			return mBuffer;
		}

		~OpenGLBuffer() override;

	protected:
		void* map_() override;

		void unmap_() override;

		uint64_t getDeviceAddress_() override;

		void copyFromBuffer_(GPUBuffer& other, uint32_t srcOffset, uint32_t dstOffset, uint32_t size) override;

		void fill_(uint32_t offset, uint32_t size, uint32_t value) override;

		void barrier_(BufferAccess srcAccess, BufferAccess dstAccess, uint32_t offset, uint32_t size) override;

	private:
		unsigned int mBuffer = 0;
	};
}
