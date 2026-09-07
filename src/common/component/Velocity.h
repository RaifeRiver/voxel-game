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

#include "glm/vec3.hpp"

#include "common/ecs/Component.h"

namespace voxel_game::component {
	struct Velocity : ecs::Component<Velocity> {
		glm::vec3 velocity;

		[[nodiscard]] operator glm::vec3() const {
			return velocity;
		}

		void operator*=(const float f) {
			velocity *= f;
		}

		void operator+=(const glm::vec3 v) {
			velocity += v;
		}

		Velocity operator*(const float deltaTime) const {
			Velocity result;
			result.velocity = velocity * deltaTime;
			return result;
		}
	};
}
