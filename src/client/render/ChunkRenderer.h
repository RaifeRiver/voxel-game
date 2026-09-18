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
	struct ChunkVertex {
		glm::vec3 position;
		uint32_t colour;
	};

	struct ChunkPushConstants {
		glm::mat4 viewProj;
		uint64_t vertexBufferAddress;
	};

	class ChunkRenderer : public ecs::System<ChunkRenderer> {
	public:
		explicit ChunkRenderer(ecs::ECSRegistry& registry);

		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<engine::RenderPipeline> mPipeline = nullptr;
		std::unique_ptr<engine::DescriptorAllocator> mDescriptorAllocator = nullptr;
		std::unique_ptr<engine::DescriptorSet> mDescriptorSet = nullptr;

		static ChunkMesh meshChunk(engine::RenderEngine& renderEngine, const chunk::Chunk& chunk);
	};
}
