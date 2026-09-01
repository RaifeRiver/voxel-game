#pragma once

#include "Chunk.h"
#include "ChunkPos.h"
#include "common/ecs/Component.h"

namespace voxel_game::chunk {
	struct ChunkData : ecs::Component<ChunkData> {
		std::unordered_map<ChunkPos, Chunk> chunks;

		bool isLoaded(ChunkPos pos) const;

		uint32_t getBlock(int32_t x, int32_t y, int32_t z);

		void setBlock(int32_t x, int32_t y, int32_t z, uint32_t block);
	};
}
