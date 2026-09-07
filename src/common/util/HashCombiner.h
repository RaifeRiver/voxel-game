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
#include <functional>

namespace voxel_game::util {
	template <typename T> void combineHash(std::size_t& seed, const T& value) {
		seed ^= std::hash<T>()(value) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
	}

	template <typename... Args> size_t hash(Args&&... args) {
		size_t seed = 0;
		(combineHash(seed, std::forward<Args>(args)), ...);
		return seed;
	}
}
