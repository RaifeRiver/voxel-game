#pragma once

#include "client/render/engine/GPUBuffer.h"

namespace voxel_game::client::render::engine::opengl {
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

	private:
		unsigned int mBuffer = 0;
	};
}
