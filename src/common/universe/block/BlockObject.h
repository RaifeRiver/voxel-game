#pragma once

#include <memory>

#include "Chunk.h"
#include "common/util/Hashers.h"

namespace voxel_game::universe::block {
	class BlockObject {
	public:
		Chunk * getChunk(const glm::ivec3& pos) const;

		bool isChunkLoaded(const glm::ivec3& pos) const;

		void loadChunk(const glm::ivec3& pos);

		void unloadChunk(const glm::ivec3& pos);

		uint32_t getBlock(const glm::ivec3& pos) const;

		void setBlock(const glm::ivec3& pos, uint32_t block) const;

	private:
		mutable std::shared_mutex mMutex;
		std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, util::ivec3Hasher> mChunks;
	};
}
