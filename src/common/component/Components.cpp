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
