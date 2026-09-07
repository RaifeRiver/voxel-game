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

#pragma once

#include "common/ecs/System.h"

namespace voxel_game::client::player {
	constexpr float DRAG_COEFFICIENT = 3.0f;
	constexpr float MOUSE_SENSITIVITY = 0.002f;
	constexpr float MOVEMENT_SPEED = 10.0f;

	class PlayerInputController : public ecs::System<PlayerInputController> {
	public:
		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;
	};
}
