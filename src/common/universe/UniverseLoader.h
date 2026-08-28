#pragma once

#include "common/ecs/Component.h"

namespace voxel_game::universe {
	struct UniverseLoader : ecs::Component<UniverseLoader> {
		float radius = 0.0f;
	};
}
