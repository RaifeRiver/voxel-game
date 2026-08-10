#pragma once

#include <cstdint>

#include "common/util/HashCombiner.h"

namespace voxel_game::universe {
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

template <> struct std::hash<voxel_game::universe::ChunkPos> {
	std::size_t operator()(const voxel_game::universe::ChunkPos& pos) const noexcept {
		return voxel_game::util::hash(pos.x, pos.y, pos.z);
	}
};