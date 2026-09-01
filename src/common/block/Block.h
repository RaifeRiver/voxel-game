#pragma once

#include "common/ecs/Component.h"

namespace voxel_game::block {
	struct Block : ecs::MarkerComponent<Block> {};
}
