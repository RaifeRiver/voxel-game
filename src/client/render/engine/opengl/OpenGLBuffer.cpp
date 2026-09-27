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

namespace voxel_game::client::render::engine::opengl {
	GLbitfield toOpenGLBufferAccess(const BufferAccess srcAccess, const BufferAccess dstAccess) {
		GLbitfield bitfield = 0;
		if (srcAccess & BufferAccess::SHADER_WRITE) {
			if (dstAccess & BufferAccess::TRANSFER_READ) {
				bitfield |= GL_BUFFER_UPDATE_BARRIER_BIT;
			}
			if (dstAccess & BufferAccess::UNIFORM_READ) {
				bitfield |= GL_UNIFORM_BARRIER_BIT;
			}
			if (dstAccess & BufferAccess::SHADER_READ || dstAccess & BufferAccess::SHADER_WRITE) {
				bitfield |= GL_SHADER_STORAGE_BARRIER_BIT;
			}
			if (dstAccess & BufferAccess::INDEX_READ) {
				bitfield |= GL_ELEMENT_ARRAY_BARRIER_BIT;
			}
			if (dstAccess & BufferAccess::INDIRECT_READ) {
				bitfield |= GL_COMMAND_BARRIER_BIT;
			}
		}
		return bitfield;
	}

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
		return glMapBuffer(GL_ARRAY_BUFFER, mMappedType == MappedType::SEQUENTIAL_WRITE ? GL_WRITE_ONLY : GL_READ_WRITE);
	}

	void OpenGLBuffer::unmap_() {
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	uint64_t OpenGLBuffer::getDeviceAddress_() {
		throw std::runtime_error("Buffer device address is not supported on OpenGL");
	}

	void OpenGLBuffer::copyFromBuffer_(GPUBuffer& other, const uint32_t srcOffset, const uint32_t dstOffset, const uint32_t size) {
		glBindBuffer(GL_COPY_READ_BUFFER, mBuffer);
		glBindBuffer(GL_COPY_WRITE_BUFFER, reinterpret_cast<OpenGLBuffer&>(other).mBuffer);
		glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);
	}

	void OpenGLBuffer::fill_(const uint32_t offset, const uint32_t size, const uint32_t value) {
		glBindBuffer(GL_ARRAY_BUFFER, mBuffer);
		glClearBufferSubData(GL_ARRAY_BUFFER, GL_R32UI, offset, size == UINT32_MAX ? mSize : size, GL_RED_INTEGER, GL_UNSIGNED_INT, &value);
	}

	void OpenGLBuffer::barrier_(const BufferAccess srcAccess, const BufferAccess dstAccess, uint32_t, uint32_t) {
		if (const GLbitfield access = toOpenGLBufferAccess(srcAccess, dstAccess)) {
			glMemoryBarrier(access);
		}
	}
}
