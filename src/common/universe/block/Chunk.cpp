#include "Chunk.h"

#include <mutex>

namespace voxel_game::universe::block {
	Chunk::Chunk(const glm::ivec3 &pos): mPos(pos) {}

	uint32_t Chunk::getBlock(const glm::ivec3 &pos) const {
		std::shared_lock lock(mMutex);
		return getBlock_(getBlockIndex(pos));
	}

	void Chunk::setBlock(const glm::ivec3 &pos, const uint32_t block) {
		std::unique_lock lock(mMutex);
		if (mBitsPerBlock == 0 && block == mBlockTypes[0].first) {
			return;
		}
		const uint32_t index = getBlockIndex(pos);
		const uint32_t oldBlock = getBlock_(index);
		if (oldBlock == block) {
			return;
		}
		getOrCreateBlockIDAndModifyCount(oldBlock, -1);
		const uint32_t id = getOrCreateBlockIDAndModifyCount(block, 1);
		const uint32_t offset = (index & mBlockMask) * mBitsPerBlock;
		mBlocks[index >> mBlockIndexShift] &= ~(mClearMask << offset);
		mBlocks[index >> mBlockIndexShift] |= static_cast<uint64_t>(id) << offset;
	}

	glm::ivec3 Chunk::getPos() const {
		return mPos;
	}

	uint32_t Chunk::getBlockIndex(const glm::ivec3& pos) {
		return pos.x & 31 | (pos.y & 31) << 5 | (pos.z & 31) << 10;
	}

	uint32_t Chunk::getBlock_(const uint32_t index) const {
		if (mBitsPerBlock == 0) {
			return mBlockTypes[0].first;
		}
		const uint32_t blockTypeIndex = mBlocks[index >> mBlockIndexShift] >> ((index & mBlockMask) * mBitsPerBlock) & mClearMask;
		if (blockTypeIndex >= mBlockTypes.size()) {
			return 0;
		}
		return mBlockTypes[blockTypeIndex].first;
	}

	uint32_t Chunk::getOrCreateBlockIDAndModifyCount(uint32_t block, int32_t countModifier) {
		for (uint32_t i = 0; i < mBlockTypes.size(); i++) {
			if (mBlockTypes[i].first == block) {
				mBlockTypes[i].second = static_cast<int16_t>(mBlockTypes[i].second + countModifier);
				if (mBlockTypes[i].second <= 0 && i != 0) {
					const uint32_t last = mBlockTypes.size() - 1;
					std::swap(mBlockTypes[i], mBlockTypes[last]);
					mBlockTypes.pop_back();
					for (uint32_t pos = 0; pos < 32768; pos++) {
						if (getBlock_(pos) == last) {
							const uint32_t offset = (pos & mBlockMask) * mBitsPerBlock;
							mBlocks[pos >> mBlockIndexShift] &= ~(mClearMask << offset);
							mBlocks[pos >> mBlockIndexShift] |= i << offset;
						}
					}
					if (mBlockTypes.size() == 1) {
						mBlocks.clear();
						mBitsPerBlock = 0;
					}
					else if (mBlockTypes.size() < 1ULL << (mBitsPerBlock >> 1)) {
						repack(mBitsPerBlock >> 1);
					}
				}
				return i;
			}
		}

		const uint32_t id = mBlockTypes.size();
		if (id >= 1ULL << mBitsPerBlock) {
			repack(mBitsPerBlock == 0? 1: mBitsPerBlock << 1);
		}
		mBlockTypes.emplace_back(block, countModifier);
		return id;
	}

	void Chunk::repack(const uint8_t bitsPerBlock) {
		const uint8_t blocksPer64Bits = 64 / bitsPerBlock;
		const uint8_t newBlockIndexShift = std::countr_zero(blocksPer64Bits);
		const uint32_t newBlockMask = blocksPer64Bits - 1;
		const uint64_t newClearMask = (1ULL << bitsPerBlock) - 1;

		const std::vector<uint64_t> oldBlocks = std::move(mBlocks);

		mBlocks.assign(512 * bitsPerBlock, 0);

		if (mBitsPerBlock > 0) {
			for (uint32_t i = 0; i < 32768; i++) {
				const uint32_t b = oldBlocks[i >> mBlockIndexShift] >> ((i & mBlockMask) * mBitsPerBlock) & mClearMask;
				if (b == 0) {
					continue;
				}
				const uint32_t offset = (i & newBlockMask) * bitsPerBlock;
				mBlocks[i >> newBlockIndexShift] |= static_cast<uint64_t>(b) << offset;
			}
		}
		else {
			for (uint32_t i = 0; i < 32768; i++) {
				const uint32_t offset = (i & newBlockMask) * bitsPerBlock;
				mBlocks[i >> newBlockIndexShift] |= static_cast<uint64_t>(mBlockTypes[0].first) << offset;
			}
		}

		mBitsPerBlock = bitsPerBlock;
		mBlockIndexShift = newBlockIndexShift;
		mBlockMask = newBlockMask;
		mClearMask = newClearMask;
	}
}
