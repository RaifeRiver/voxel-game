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
