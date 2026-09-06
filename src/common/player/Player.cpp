#include "Player.h"

#include "CameraRotation.h"
#include "common/component/Transform.h"
#include "common/component/Velocity.h"

namespace voxel_game::player {
	void attachPlayerComponents(ecs::ECSRegistry& registry, const ecs::Entity entity, const bool local) {
		registry.attachComponent<Player>(entity);
		registry.attachComponent<CameraRotation>(entity);
		registry.attachComponent<component::Transform>(entity);
		registry.attachComponent<component::Velocity>(entity);

		if (local) {
			registry.attachComponent<LocalPlayer>(entity);
		}
	}
}
