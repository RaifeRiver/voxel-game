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

#include "VoxelGame.h"

#include "tracy/Tracy.hpp"

#include "block/BlockLoader.h"
#include "component/Components.h"
#include "resource/ResourceManager.h"
#include "universe/UniverseLoader.h"

namespace voxel_game {
	void preLoad(ecs::ECSRegistry& registry) {
		ZoneScopedN("Pre init common");

		component::registerComponents(registry);

		registry.createResource<resource::ResourceManager>();
	}

	void load(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init common");

		block::loadBlocks(registry);
	}

	void postLoad(ecs::ECSRegistry& registry) {
		ZoneScopedN("Post init common");

		registry.getSystemManager().createSystem<universe::UniverseLoader>();
	}

	void destroy(ecs::ECSRegistry& registry) {

	}
}
