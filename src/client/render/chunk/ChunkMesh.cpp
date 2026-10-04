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

#include "ChunkMesh.h"

#include "tracy/Tracy.hpp"

namespace voxel_game::client::render::chunk {
	ChunkMesh meshChunk(const voxel_game::chunk::Chunk& chunk) {
		ZoneScopedN("Mesh chunk");

		ChunkMesh mesh;
		if (!chunk.isUniform() || chunk.getBlock(0, 0, 0) != 0) {
			std::vector<Face> faces;
			for (uint32_t x = 0; x < voxel_game::chunk::CHUNK_SIZE; x++) {
				for (uint32_t y = 0; y < voxel_game::chunk::CHUNK_SIZE; y++) {
					for (uint32_t z = 0; z < voxel_game::chunk::CHUNK_SIZE; z++) {
						const uint32_t block = chunk.getBlock(x, y, z);
						if (block != 0) {
							const glm::u8vec4 colour = {(block - 1) % 1024 / 32 / 31.0f * 255, (block - 1) % 32 / 31.0f * 255, (block - 1) / 1024 / 31.0f * 255, 255};

							if (z == voxel_game::chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y, z + 1) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::Z_PLUS, .colour = colour});
							}

							if (z == 0 || chunk.getBlock(x, y, z - 1) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::Z_MINUS, .colour = colour});
							}

							if (y == voxel_game::chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y + 1, z) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::Y_PLUS, .colour = colour});
							}

							if (y == 0 || chunk.getBlock(x, y - 1, z) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::Y_MINUS, .colour = colour});
							}

							if (x == voxel_game::chunk::CHUNK_SIZE - 1 || chunk.getBlock(x + 1, y, z) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::X_PLUS, .colour = colour});
							}

							if (x == 0 || chunk.getBlock(x - 1, y, z) == 0) {
								faces.push_back({.pos = {x, y, z}, .dir = Direction::X_MINUS, .colour = colour});
							}
						}
					}
				}
			}

			mesh.faces = std::move(faces);
		}

		return mesh;
	}
}
