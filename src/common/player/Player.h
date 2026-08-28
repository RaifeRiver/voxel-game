#pragma once

#include "common/ecs/Component.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::player {
	struct Player : ecs::Component<Player> {};

	struct LocalPlayer : ecs::Component<LocalPlayer> {};

	void attachPlayerComponents(ecs::ECSRegistry& registry, ecs::Entity entity, bool local = false);
}
