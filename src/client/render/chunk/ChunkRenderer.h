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
#include "client/render/engine/DescriptorAllocator.h"
#include "client/render/engine/RenderEngine.h"
#include "client/render/engine/RenderPipeline.h"
#include "common/chunk/Chunk.h"
#include "common/ecs/System.h"

namespace voxel_game::client::render::chunk {
	struct alignas(8) Chunk {
		union {
			uint64_t faceBuffer;
			struct {
				uint32_t faceOffset;
				uint32_t padding;
			};
		};
		uint32_t vertexCount;
	};

	struct FrameChunk {
		glm::mat4 modelMatrix;
		glm::ivec4 boundingSphere;
	};

	struct CullingPushConstants {
		glm::vec4 frustumPlanes[6];
		uint32_t chunkCount;
	};

	struct PushConstants {
		glm::mat4 viewProj;
	};

	struct ChunkRender {
		uint32_t chunkIndex = 0;
		std::unique_ptr<engine::GPUBuffer> vertexBuffer = nullptr;
	};

	struct ChunkRenderData : ecs::Component<ChunkRenderData> {
		std::unordered_map<glm::ivec3, ChunkRender> data;
		std::vector<glm::ivec3> positions;
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
		std::unique_ptr<engine::GPUBuffer> mFrameChunkBuffers[2] = {};
		std::unique_ptr<engine::GPUBuffer> mIndirectCommandBuffer = nullptr;
		std::unique_ptr<engine::GPUBuffer> mCountBuffer = nullptr;
		std::unique_ptr<engine::GPUBuffer> mFaceBuffer = nullptr;
		uint32_t mFaceBufferPointer = 0;
		uint32_t mNextChunk = 0;
		bool mUseBufferReference = false;
	};
}
