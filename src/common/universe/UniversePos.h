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

#include "glm/vec3.hpp"

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

		void operator+=(const glm::vec3& other) {
			local += other;
			fix();
		}
	};

	[[nodiscard]] inline UniversePos operator+(const UniversePos& a, const UniversePos& b) {
		UniversePos result = {};
		result.sector = a.sector + b.sector;
		result.local = a.local + b.local;
		result.fix();
		return result;
	}

	[[nodiscard]] inline UniversePos operator+(const UniversePos& a, const glm::vec3& b) {
		UniversePos result = {};
		result.local = a.local + b;
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