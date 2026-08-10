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
