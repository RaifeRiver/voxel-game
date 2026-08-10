#include "ChunkData.h"

#include <stdexcept>

namespace voxel_game::universe {
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
