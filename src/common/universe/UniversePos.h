#pragma once

#include "glm/vec3.hpp"

namespace voxel_game::universe {
	static uint32_t SECTOR_SIZE = 64;

	struct UniversePos {
		glm::i64vec3 sector;
		glm::vec3 local;
	};
}