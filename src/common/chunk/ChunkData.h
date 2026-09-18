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

#include "glm/gtx/hash.hpp"
#include "glm/vec3.hpp"

#include "Chunk.h"
#include "common/ecs/Component.h"

namespace voxel_game::chunk {
	struct ChunkData : ecs::Component<ChunkData> {
		std::unordered_map<glm::ivec3, Chunk> chunks;

		bool isLoaded(glm::ivec3 pos) const;

		uint32_t getBlock(int32_t x, int32_t y, int32_t z);

		void setBlock(int32_t x, int32_t y, int32_t z, uint32_t block);
	};
}
