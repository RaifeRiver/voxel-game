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

#include "Player.h"

#include "CameraRotation.h"
#include "common/component/Transform.h"
#include "common/component/Velocity.h"
#include "common/universe/UniverseLoaderInfo.h"

namespace voxel_game::player {
	void attachPlayerComponents(ecs::ECSRegistry& registry, const ecs::Entity entity, const bool local) {
		registry.attachComponent<Player>(entity);
		registry.attachComponent<CameraRotation>(entity);
		registry.attachComponent<component::Transform>(entity);
		registry.attachComponent<component::Velocity>(entity);
		registry.attachComponent<universe::UniverseLoaderInfo>(entity).radius = 8;

		if (local) {
			registry.attachComponent<LocalPlayer>(entity);
		}
	}
}
