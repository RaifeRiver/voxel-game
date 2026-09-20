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

#include "glm/mat4x4.hpp"

#include "ChunkMesh.h"
#include "common/chunk/Chunk.h"
#include "common/ecs/System.h"
#include "engine/DescriptorAllocator.h"
#include "engine/RenderEngine.h"
#include "engine/RenderPipeline.h"

namespace voxel_game::client::render {
	struct alignas(16) Chunk {
		glm::mat4 modelMatrix;
		glm::ivec4 boundingSphere;
		uint64_t bufferAddress;
		uint32_t vertexCount;
	};

	struct Vertex {
		glm::u8vec3 pos;
		uint8_t dir;
		uint32_t colour;
	};

	struct CullingPushConstants {
		glm::vec4 frustumPlanes[6];
		uint32_t chunkCount;
	};

	struct PushConstants {
		glm::mat4 viewProj;
	};

	class ChunkRenderer : public ecs::System<ChunkRenderer> {
	public:
		explicit ChunkRenderer(ecs::ECSRegistry& registry);

		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<engine::DescriptorLayout> mDescriptorLayout = nullptr;
		std::unique_ptr<engine::DescriptorAllocator> mDescriptorAllocator = nullptr;
		std::unique_ptr<engine::DescriptorSet> mDescriptorSet = nullptr;
		std::unique_ptr<engine::ComputePipeline> mCullingPipeline = nullptr;
		std::unique_ptr<engine::RenderPipeline> mRenderPipeline = nullptr;
		std::unique_ptr<engine::GPUBuffer> mChunkBuffer = nullptr;
		std::unique_ptr<engine::GPUBuffer> mIndirectCommandBuffer = nullptr;
		std::unique_ptr<engine::GPUBuffer> mCountBuffer = nullptr;
		uint32_t mNextChunk = 0;

		ChunkMesh meshChunk(engine::RenderEngine& renderEngine, const chunk::Chunk& chunk);
	};
}
