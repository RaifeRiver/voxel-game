#pragma once

#include "Registry.h"
#include "common/block/Block.h"

namespace voxel_game::registry {
	inline Registry<block::Block> BLOCKS;

	inline const block::Block* AIR = BLOCKS.registerObject({"voxel_game", "air"}, std::make_unique<block::Block>());
	inline const block::Block* DIRT = BLOCKS.registerObject({"voxel_game", "dirt"}, std::make_unique<block::Block>());
	inline const block::Block* GRASS = BLOCKS.registerObject({"voxel_game", "grass"}, std::make_unique<block::Block>());
}
