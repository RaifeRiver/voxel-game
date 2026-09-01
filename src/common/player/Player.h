#pragma once

#include "common/ecs/Component.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::player {
	struct Player : ecs::MarkerComponent<Player> {};

	struct LocalPlayer : ecs::MarkerComponent<LocalPlayer> {};

	void attachPlayerComponents(ecs::ECSRegistry& registry, ecs::Entity entity, bool local = false);
}
