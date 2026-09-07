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

#include "OpenGLBuffer.h"

#include <stdexcept>

#include "glad/glad.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLBuffer::OpenGLBuffer(const size_t size, const BufferUsage usage, const MemoryType memoryType, const MappedType mappedType) : GPUBuffer(size, usage, memoryType, mappedType) {
		glGenBuffers(1, &mBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
		glBufferData(GL_ARRAY_BUFFER, static_cast<long>(size), nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLBuffer::~OpenGLBuffer() {
		glDeleteBuffers(1, &mBuffer);
	}

	void* OpenGLBuffer::map_() {
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
		return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
	}

	void OpenGLBuffer::unmap_() {
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	uint64_t OpenGLBuffer::getDeviceAddress_() {
		throw std::runtime_error("Buffer device address is not supported on OpenGL");
	}
}
