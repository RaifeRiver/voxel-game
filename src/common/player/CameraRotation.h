#pragma once

#include "common/ecs/Component.h"

namespace voxel_game::player {
	struct CameraRotation : ecs::Component<CameraRotation> {
		float yaw;
		float pitch;
	};
}
