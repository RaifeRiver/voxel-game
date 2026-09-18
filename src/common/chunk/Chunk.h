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

#include <cmath>
#include <vector>

#include "glm/vec3.hpp"

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
		Chunk(glm::ivec3 pos, ecs::Entity object);

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

		[[nodiscard]] glm::ivec3 getPos() const {
			return mPos;
		}

	private:
		uint32_t mUniformType = 0;
		uint8_t mBitsPerBlock = 0;
		uint8_t mIndexShift = 0;
		uint32_t mBlockMask = 0;
		uint64_t mClearMask = 0;
		std::vector<uint64_t> mData;
		std::vector<ChunkPaletteEntry> mPalette;
		bool mPaletted = true;
		glm::ivec3 mPos;
		ecs::Entity mObject;

		void convertToPalette();

		uint32_t getPaletteIDModifyCount(uint32_t id, int32_t countModifier);

		[[nodiscard]] uint32_t getShrinkThreshold() const;

		uint32_t addToPalette(uint32_t id);

		void removeFromPalette(uint32_t id);

		void resizeData(uint8_t bitsPerBlock);
	};
}
