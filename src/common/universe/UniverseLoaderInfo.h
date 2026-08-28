#pragma once

#include "common/ecs/Component.h"

namespace voxel_game::universe {
	struct UniverseLoaderInfo : ecs::Component<UniverseLoaderInfo> {
		float radius = 0.0f;
	};
}
