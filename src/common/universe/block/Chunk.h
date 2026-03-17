#pragma once

#include <atomic>
#include <cstdint>
#include <shared_mutex>
#include <vector>

#include "glm/vec3.hpp"

namespace voxel_game::universe::block {
	class Chunk {
	public:
		explicit Chunk(const glm::ivec3& pos);

		[[nodiscard]] uint32_t getBlock(const glm::ivec3& pos) const;

		void setBlock(const glm::ivec3& pos, uint32_t block);

		[[nodiscard]] glm::ivec3 getPos() const;

	private:
		glm::ivec3 mPos;
		mutable std::shared_mutex mMutex;
		std::vector<uint64_t> mBlocks;
		std::vector<std::pair<uint32_t, int32_t>> mBlockTypes = {{0, 32 * 32 * 32}};
		uint8_t mBitsPerBlock = 0;
		uint8_t mBlockIndexShift = 0;
		uint32_t mBlockMask = 0;
		uint64_t mClearMask = 0;

		[[nodiscard]] static inline uint32_t getBlockIndex(const glm::ivec3& pos);

		[[nodiscard]] uint32_t getBlock_(uint32_t index) const;

		uint32_t getOrCreateBlockIDAndModifyCount(uint32_t block, int32_t countModifier);

		void repack(uint8_t bitsPerBlock);
	};
}
