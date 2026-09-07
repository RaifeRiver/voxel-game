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