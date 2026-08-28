#include "Player.h"

#include "common/universe/Transform.h"
#include "common/universe/UniverseLoader.h"

namespace voxel_game::player {
	void attachPlayerComponents(ecs::ECSRegistry& registry, const ecs::Entity entity, const bool local) {
		registry.attachComponent<Player>(entity);
		registry.attachComponent<universe::Transform>(entity);
		registry.attachComponent<universe::UniverseLoader>(entity);

		if (local) {
			registry.attachComponent<LocalPlayer>(entity);
		}
	}
}
