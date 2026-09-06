#pragma once

#include "glm/vec3.hpp"

#include "HashCombiner.h"

template <typename T> struct std::hash<glm::vec<3, T>> {
	std::size_t operator()(const glm::vec<3, T>& v) const noexcept {
		return voxel_game::util::hash(v.x, v.y, v.z);
	}
};