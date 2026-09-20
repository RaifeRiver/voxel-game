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

#include <memory>

#include "ComputePipeline.h"
#include "GPUBuffer.h"
#include "GPUImage.h"
#include "RenderPipeline.h"
#include "common/ecs/ECSRegistry.h"
#include "common/ecs/Resource.h"

namespace voxel_game::client::render::engine {
	constexpr uint32_t FRAME_OVERLAP = 2;

	class RenderEngine : public ecs::Resource<RenderEngine> {
	public:
		virtual std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) = 0;

		std::unique_ptr<GPUBuffer> allocateBuffer(const size_t size, const BufferUsage usage, const MemoryType memoryType, const MappedType mappedType = MappedType::NONE) {
			return allocateBuffer_(size, usage, memoryType, mappedType);
		}

		virtual std::unique_ptr<ComputePipelineBuilder> createComputePipelineBuilder(const Shader& computeShader) = 0;

		virtual std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader) = 0;

		virtual std::unique_ptr<DescriptorLayoutBuilder> createDescriptorLayoutBuilder() = 0;

		virtual GPUImage& getRenderImage() = 0;

		virtual GPUImage& getDepthImage() = 0;

		virtual void beginRendering() = 0;

		virtual void endRendering() = 0;

		virtual void waitForGPU() = 0;

		[[nodiscard]] uint64_t getFrame() const {
			return mFrame;
		}

	protected:
		uint64_t mFrame = 0;

		virtual std::unique_ptr<GPUBuffer> allocateBuffer_(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType) = 0;
	};
}
