#include "Components.h"

#include "TextID.h"
#include "Transform.h"
#include "common/block/Block.h"
#include "common/chunk/ChunkData.h"
#include "common/player/Player.h"
#include "common/universe/UniverseLoaderInfo.h"

namespace voxel_game::component {
	void registerComponents(ecs::ECSRegistry& registry) {
		registry.registerComponentType<block::Block>("voxel_game:block");

		registry.registerComponentType<chunk::ChunkData>("voxel_game:chunk_data");

		registry.registerComponentType<TextID>("voxel_game:text_id");
		registry.registerComponentType<Transform>("voxel_game:transform");

		registry.registerComponentType<player::Player>("voxel_game:player");
		registry.registerComponentType<player::LocalPlayer>("voxel_game:local_player");

		registry.registerComponentType<universe::UniverseLoaderInfo>("voxel_game:universe_loader_info");
	}
}
