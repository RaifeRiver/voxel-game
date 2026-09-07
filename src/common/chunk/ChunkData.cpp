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

#include "ChunkData.h"

#include <stdexcept>

namespace voxel_game::chunk {
	bool ChunkData::isLoaded(const ChunkPos pos) const {
		return chunks.contains(pos);
	}

	uint32_t ChunkData::getBlock(const int32_t x, const int32_t y, const int32_t z) {
		const auto it = chunks.find({x >> CHUNK_SHIFT, y >> CHUNK_SHIFT, z >> CHUNK_SHIFT});
		if (it != chunks.end()) {
			return it->second.getBlock(x & CHUNK_MASK, y & CHUNK_MASK, z & CHUNK_MASK);
		}
		throw std::runtime_error("Chunk not loaded");
	}

	void ChunkData::setBlock(const int32_t x, const int32_t y, const int32_t z, const uint32_t block) {
		const auto it = chunks.find({x >> CHUNK_SHIFT, y >> CHUNK_SHIFT, z >> CHUNK_SHIFT});
		if (it != chunks.end()) {
			it->second.setBlock(x & CHUNK_MASK, y & CHUNK_MASK, z & CHUNK_MASK, block);
		}
		throw std::runtime_error("Chunk not loaded");
	}
}
