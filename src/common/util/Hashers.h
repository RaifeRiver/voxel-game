#pragma once
#include <cstddef>
#include <functional>

#include "glm/vec3.hpp"

namespace voxel_game::util {
	template <class T> void combineHash(std::size_t& seed, const T& v) {
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	struct i64vec3Hasher {
		std::size_t operator()(const glm::i64vec3& v) const {
			size_t seed = 0;
			combineHash(seed, v.x);
			combineHash(seed, v.y);
			combineHash(seed, v.z);
			return seed;
		}
	};
}
