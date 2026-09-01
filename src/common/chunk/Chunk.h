#pragma once

#include <cmath>
#include <vector>

#include "ChunkPos.h"
#include "common/ecs/Component.h"
#include "common/ecs/Entity.h"

namespace voxel_game::chunk {
	constexpr uint32_t CHUNK_SIZE = 32;
	constexpr uint32_t CHUNK_SIZE2 = CHUNK_SIZE * CHUNK_SIZE;
	constexpr uint32_t CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
	const uint32_t CHUNK_SHIFT = static_cast<uint32_t>(std::log2(CHUNK_SIZE));
	constexpr uint32_t CHUNK_MASK = CHUNK_SIZE - 1;

	struct ChunkPaletteEntry {
		uint32_t id;
		int32_t count;
	};

	class Chunk {
	public:
		Chunk(ChunkPos pos, ecs::Entity object);

		[[nodiscard]] uint32_t getBlock(const uint32_t x, const uint32_t y, const uint32_t z) const {
			return getBlock(getIndex(x, y, z));
		}

		[[nodiscard]] uint32_t getBlock(uint32_t index) const;

		void setBlock(const uint32_t x, const uint32_t y, const uint32_t z, const uint32_t id) {
			setBlock(getIndex(x, y, z), id);
		}

		void setBlock(uint32_t index, uint32_t id);

		[[nodiscard]] bool isUniform() const;

		[[nodiscard]] static uint32_t getIndex(const uint32_t x, const uint32_t y, const uint32_t z) {
			return x + y * CHUNK_SIZE + z * CHUNK_SIZE2;
		}

		[[nodiscard]] ChunkPos getPos() const {
			return mPos;
		}

	private:
		uint32_t mUniformType = 0;
		uint8_t mBitsPerBlock = 0;
		uint8_t mIndexShift = 0;
		uint32_t mBlockMask = 0;
		uint32_t mClearMask = 0;
		std::vector<uint64_t> mData;
		std::vector<ChunkPaletteEntry> mPalette;
		ChunkPos mPos;
		ecs::Entity mObject;

		void convertToPalette(uint32_t index, uint32_t id);

		uint32_t getPaletteIDModifyCount(uint32_t id, int32_t countModifier);

		[[nodiscard]] uint32_t getShrinkThreshold() const;

		uint32_t addToPalette(uint32_t id);

		void removeFromPalette(uint32_t id);

		void resizeData(uint8_t bitsPerBlock);
	};
}
