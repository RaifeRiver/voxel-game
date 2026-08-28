#include "Player.h"

#include "common/component/Transform.h"
#include "common/component/UniverseLoadInfo.h"

namespace voxel_game::player {
	void attachPlayerComponents(ecs::ECSRegistry& registry, const ecs::Entity entity, const bool local) {
		registry.attachComponent<Player>(entity);
		registry.attachComponent<universe::Transform>(entity);
		registry.attachComponent<universe::UniverseLoadInfo>(entity);

		if (local) {
			registry.attachComponent<LocalPlayer>(entity);
		}
	}
}
