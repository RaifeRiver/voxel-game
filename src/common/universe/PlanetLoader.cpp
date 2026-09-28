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

#include "PlanetLoader.h"

#include "tracy/Tracy.hpp"

#include "common/chunk/ChunkData.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadPlanetEvent.h"
#include "common/event/LoadSectorEvent.h"
#include "common/util/Log.h"

namespace voxel_game::universe {
	void PlanetLoader::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		ZoneScopedN("Generating planets");

		const std::vector<event::LoadSectorEvent*> loadSectorEvents = registry.getEvents<event::LoadSectorEvent>();
		for (const event::LoadSectorEvent* loadSectorEvent : loadSectorEvents) {
			if (loadSectorEvent->sector.x == 0 && loadSectorEvent->sector.y == 0 && loadSectorEvent->sector.z == 0) {
				const ecs::Entity planet = registry.createEntity();
				registry.attachComponent<component::Transform>(planet);
				registry.attachComponent<chunk::ChunkData>(planet);
				registry.pushEvent<event::LoadPlanetEvent>(planet);
			}
		}
	}
}
