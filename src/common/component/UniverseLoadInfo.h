#pragma once

#include "common/ecs/Component.h"

namespace voxel_game::universe {
	struct UniverseLoadInfo : ecs::Component<UniverseLoadInfo> {
		float radius = 0.0f;
	};
}
