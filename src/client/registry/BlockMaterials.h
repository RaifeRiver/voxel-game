#pragma once

#include "client/renderer/universe/BlockMaterial.h"
#include "common/registry/Registry.h"

namespace voxel_game::client::registry::block_materials {
	inline voxel_game::registry::Registry<renderer::universe::BlockMaterial> BLOCK_MATERIALS;

	inline renderer::universe::BlockMaterial* const DIRT = BLOCK_MATERIALS.registerObject({"voxel_game", "dirt"}, std::make_unique<renderer::universe::BlockMaterial>("voxel_game:dirt_block"));
	inline renderer::universe::BlockMaterial* const GRASS = BLOCK_MATERIALS.registerObject({"voxel_game", "grass"}, std::make_unique<renderer::universe::BlockMaterial>("voxel_game:grass_block"));
}
