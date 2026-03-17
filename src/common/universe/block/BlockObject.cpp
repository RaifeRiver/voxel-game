#include "BlockObject.h"

#include <mutex>

namespace voxel_game::universe::block {
	Chunk * BlockObject::getChunk(const glm::ivec3 &pos) const {
		std::shared_lock lock(mMutex);
		const auto it = mChunks.find(pos);
		return it == mChunks.end()? nullptr: it->second.get();
	}

	bool BlockObject::isChunkLoaded(const glm::ivec3 &pos) const {
		std::shared_lock lock(mMutex);
		return mChunks.contains(pos);
	}

	void BlockObject::loadChunk(const glm::ivec3 &pos) {
		std::unique_lock lock(mMutex);
		mChunks[pos] = std::make_unique<Chunk>(pos);
	}

	void BlockObject::unloadChunk(const glm::ivec3 &pos) {
		std::unique_lock lock(mMutex);
		mChunks.erase(pos);
	}

	uint32_t BlockObject::getBlock(const glm::ivec3 &pos) const {
		const Chunk *chunk = getChunk(pos >> 5);
		if (!chunk) {
			return 0;
		}
		return chunk->getBlock(pos & 31);
	}

	void BlockObject::setBlock(const glm::ivec3 &pos, const uint32_t block) const {
		Chunk *chunk = getChunk(pos >> 5);
		if (chunk) {
			chunk->setBlock(pos & 31, block);
		}
	}
}
