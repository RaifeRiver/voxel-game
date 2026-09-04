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
