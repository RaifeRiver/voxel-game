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

namespace voxel_game::util {
    template <typename T> bool checkPointIntersectsSphere(glm::vec<3, T> centre, T radius, glm::vec<3, T> point) {
		const glm::vec<3, T> distance = point - centre;
    	const T distance2 = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
    	const T radius2 = radius * radius;
    	return distance2 <= radius2;
    }

	template <typename T> bool checkSpheresIntersect(glm::vec<3, T> centre1, T radius1, glm::vec<3, T> centre2, T radius2) {
	    const glm::vec<3, T> distance = centre1 - centre2;
    	const T distance2 = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
    	const T radiusSum = radius1 + radius2;
    	const T radiusSum2 = radiusSum * radiusSum;
    	return distance2 <= radiusSum2;
    }
}