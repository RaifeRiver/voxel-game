#pragma once

#include "glm/vec3.hpp"
#include "glm/geometric.hpp"

namespace voxel_game::universe {
	constexpr static int32_t SECTOR_SIZE = 64;

	struct UniversePos {
		glm::i64vec3 sector;
		glm::vec3 local;

		void fix() {
			const auto iLocal = glm::ivec3(local);
			const glm::ivec3 sectors = iLocal / SECTOR_SIZE;
			if (sectors != glm::ivec3(0)) {
				sector += sectors;
				local -= sectors * SECTOR_SIZE;
			}
		}

		[[nodiscard]] double length() const {
			return glm::length(glm::dvec3(sector)) * SECTOR_SIZE + glm::length(local);
		}
	};

	[[nodiscard]] inline UniversePos operator+(const UniversePos& a, const UniversePos& b) {
		UniversePos result = {};
		result.sector = a.sector + b.sector;
		result.local = a.local + b.local;
		result.fix();
		return result;
	}

	[[nodiscard]] inline UniversePos operator-(const UniversePos& a, const UniversePos& b) {
		UniversePos result = {};
		result.sector = a.sector - b.sector;
		result.local = a.local - b.local;
		result.fix();
		return result;
	}
}