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

#include "glm/vec3.hpp"
#include "glm/gtx/hash.hpp"

#include "client/render/engine/GPUBuffer.h"
#include "common/chunk/Chunk.h"

namespace voxel_game::client::render::chunk {
	enum class Direction : uint8_t {
		Z_PLUS = 0,
		Z_MINUS = 1,
		Y_PLUS = 2,
		Y_MINUS = 3,
		X_PLUS = 4,
		X_MINUS = 5
	};

	struct Face {
		glm::u8vec3 pos;
		Direction dir;
		glm::u8vec4 colour;
	};

	struct ChunkMesh {
		std::vector<Face> faces;
	};;

	ChunkMesh meshChunk(const voxel_game::chunk::Chunk& chunk);
}
