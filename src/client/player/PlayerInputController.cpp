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

#include "PlayerInputController.h"

#include "tracy/Tracy.hpp"

#include "client/window/Window.h"
#include "common/component/Transform.h"
#include "common/component/Velocity.h"
#include "common/ecs/Entity.h"
#include "common/player/CameraRotation.h"
#include "common/player/Player.h"

namespace voxel_game::client::player {
	void PlayerInputController::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, const float deltaTime) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		ZoneScopedN("Process player input");

		window::Window& window = registry.getResource<window::Window>();

		const std::vector<ecs::Entity> players = registry.getEntitiesWithComponents<voxel_game::player::LocalPlayer, component::Transform, component::Velocity, voxel_game::player::CameraRotation>();
		for (const ecs::Entity entity: players) {
			component::Transform& transform = registry.getComponent<component::Transform>(entity);
			component::Velocity& velocity = registry.getComponent<component::Velocity>(entity);
			voxel_game::player::CameraRotation& rotation = registry.getComponent<voxel_game::player::CameraRotation>(entity);

			const glm::vec2 mouseMovement = window.getMouseMovement();
			rotation.yaw += mouseMovement.x * MOUSE_SENSITIVITY;
			rotation.pitch -= mouseMovement.y * MOUSE_SENSITIVITY;
			rotation.pitch = glm::clamp(rotation.pitch, -1.5f, 1.5f);

			glm::vec3 movementDirection = {};
			if (window.isKeyPressed(window::Key::KEY_W)) {
				movementDirection.z -= 1;
			}
			if (window.isKeyPressed(window::Key::KEY_S)) {
				movementDirection.z += 1;
			}
			if (window.isKeyPressed(window::Key::KEY_A)) {
				movementDirection.x -= 1;
			}
			if (window.isKeyPressed(window::Key::KEY_D)) {
				movementDirection.x += 1;
			}
			if (glm::length2(movementDirection) > 0.0f) {
				movementDirection = glm::normalize(movementDirection);
				glm::quat yawQuat = glm::angleAxis(rotation.yaw, glm::vec3(0.0f, -1.0f, 0.0f));
				velocity += yawQuat * movementDirection * MOVEMENT_SPEED * DRAG_COEFFICIENT * deltaTime;
			}
			if (window.isKeyPressed(window::Key::KEY_SPACE)) {
				movementDirection.y += 1;
			}
			if (window.isKeyPressed(window::Key::KEY_LEFT_SHIFT)) {
				movementDirection.y -= 1;
			}
			velocity.velocity.y += movementDirection.y * MOVEMENT_SPEED * DRAG_COEFFICIENT * deltaTime;
			velocity *= std::exp(-DRAG_COEFFICIENT * deltaTime);

			transform.pos += velocity * deltaTime;
		}
	}
}
