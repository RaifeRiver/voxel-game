#include "Chunk.h"

namespace voxel_game::chunk {
	Chunk::Chunk(const ChunkPos pos, const ecs::Entity object) : mPos(pos), mObject(object) {}

	uint32_t Chunk::getBlock(const uint32_t index) const {
		if (mBitsPerBlock == 0) {
			return mUniformType;
		}
		return mPalette[(mData[index >> mIndexShift] >> ((index & mBlockMask) * mBitsPerBlock)) & mClearMask].id;
	}

	void Chunk::setBlock(const uint32_t index, const uint32_t id) {
		const uint32_t currentBlock = getBlock(index);
		if (currentBlock == id) {
			return;
		}
		getPaletteIDModifyCount(currentBlock, -1);
		const uint32_t paletteID = getPaletteIDModifyCount(id, 1);
		if (mBitsPerBlock == 0) {
			return;
		}
		const uint32_t offset = (index & mBlockMask) * mBitsPerBlock;
		const uint32_t shiftedIndex = index >> mIndexShift;
		mData[shiftedIndex] &= ~(mClearMask << offset);
		mData[shiftedIndex] |= static_cast<uint64_t>(paletteID) << offset;
	}

	bool Chunk::isUniform() const {
		return mBitsPerBlock == 0;
	}

	void Chunk::convertToPalette() {
		mBitsPerBlock = 1;
		mIndexShift = 6;
		mBlockMask = 63;
		mClearMask = 1;
		mData.resize(CHUNK_VOLUME >> 6, 0);
		mPalette.resize(1);
		mPalette[0] = {.id = mUniformType, .count = CHUNK_VOLUME};
	}

	uint32_t Chunk::getPaletteIDModifyCount(const uint32_t id, const int32_t countModifier) {
		if (mBitsPerBlock == 0) {
			if (id != mUniformType) {
				convertToPalette();
				mPalette.emplace_back(id, countModifier);
				return 1;
			}
			return 0;
		}
		for (size_t i = 0; i < mPalette.size(); i++) {
			if (mPalette[i].id == id) {
				mPalette[i].count += countModifier;
				if (mPalette[i].count <= 0) {
					removeFromPalette(id);
				}
				return i;
			}
		}
		const uint32_t paletteID = addToPalette(id);
		mPalette[paletteID].count += countModifier;
		return paletteID;
	}

	uint32_t Chunk::getShrinkThreshold() const {
		switch (mBitsPerBlock) {
			case 2:
				return 2;
			case 4:
				return 3;
			case 8:
				return 12;
			case 16:
				return 240;
			default:
				return UINT32_MAX;
		};
	}

	uint32_t Chunk::addToPalette(const uint32_t id) {
		const uint32_t paletteID = mPalette.size();
		if (paletteID > (1 << mBitsPerBlock) - 1) {
			resizeData(mBitsPerBlock << 1);
		}
		mPalette.push_back({.id = id, .count = 0});
		return paletteID;
	}

	void Chunk::removeFromPalette(const uint32_t id) {
		const uint32_t last = mPalette.size() - 1;
		std::swap(mPalette[last], mPalette[id]);
		mPalette.pop_back();
		if (mPalette.size() == 1) {
			mUniformType = last;
			mBitsPerBlock = 0;
			mPalette.clear();
			mPalette.shrink_to_fit();
			mData.clear();
			mData.shrink_to_fit();
		}
		else {
			for (uint32_t index = 0; index < CHUNK_VOLUME; index++) {
				const uint32_t offset = (index & mBlockMask) * mBitsPerBlock;
				const uint32_t shiftedIndex = index >> mIndexShift;
				if (((mData[shiftedIndex] >> offset) & mClearMask) == last) {
					mData[shiftedIndex] &= ~(mClearMask << offset);
					mData[shiftedIndex] |= id << offset;
				}
			}
			if (mPalette.size() <= getShrinkThreshold()) {
				resizeData(mBitsPerBlock >> 1);
				mData.shrink_to_fit();
			}
		}
	}

	void Chunk::resizeData(const uint8_t bitsPerBlock) {
		const uint8_t blocksPer64Bits = 64 / bitsPerBlock;
		const uint8_t indexShift = std::countr_zero(blocksPer64Bits);
		const uint32_t blockMask = blocksPer64Bits - 1;
		const uint64_t clearMask = (1 << bitsPerBlock) - 1;

		const std::vector<uint64_t> data = std::move(mData);
		mData.resize((CHUNK_VOLUME >> 6) * bitsPerBlock, 0);

		for (uint32_t i = 0; i < CHUNK_VOLUME; i++) {
			const uint32_t offset = (i & mBlockMask) * mBitsPerBlock;
			const uint32_t id = (data[i >> mIndexShift] >> offset) & mClearMask;
			if (id != 0) {
				mData[i >> indexShift] |= id << offset;
			}
		}

		mBitsPerBlock = bitsPerBlock;
		mIndexShift = indexShift;
		mBlockMask = blockMask;
		mClearMask = clearMask;
	}
}
