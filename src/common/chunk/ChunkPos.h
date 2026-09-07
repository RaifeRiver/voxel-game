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

#include <cstdint>

#include "common/util/HashCombiner.h"

namespace voxel_game::chunk {
	struct ChunkPos {
		int32_t x;
		int32_t y;
		int32_t z;

		ChunkPos() : x(0), y(0), z(0) {}

		ChunkPos(const int32_t x, const int32_t y, const int32_t z) : x(x), y(y), z(z) {}

		bool operator==(const ChunkPos& other) const {
			return x == other.x && y == other.y && z == other.z;
		}
	};
}

template <> struct std::hash<voxel_game::chunk::ChunkPos> {
	std::size_t operator()(const voxel_game::chunk::ChunkPos& pos) const noexcept {
		return voxel_game::util::hash(pos.x, pos.y, pos.z);
	}
};