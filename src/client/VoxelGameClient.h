#pragma once

#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry);

	void run(ecs::ECSRegistry& registry);

	void destroy(ecs::ECSRegistry& registry);
}
