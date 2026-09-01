#include "Player.h"

#include "common/component/Transform.h"
#include "common/universe/UniverseLoaderInfo.h"

namespace voxel_game::player {
	void attachPlayerComponents(ecs::ECSRegistry& registry, const ecs::Entity entity, const bool local) {
		registry.attachComponent<Player>(entity);
		registry.attachComponent<component::Transform>(entity);
		registry.attachComponent<universe::UniverseLoaderInfo>(entity).radius = 100;

		if (local) {
			registry.attachComponent<LocalPlayer>(entity);
		}
	}
}
