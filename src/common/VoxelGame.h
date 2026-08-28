#pragma once

#include "ecs/ECSRegistry.h"

namespace voxel_game {
	void preLoad(ecs::ECSRegistry& registry);

	void load(ecs::ECSRegistry& registry);

	void postLoad(ecs::ECSRegistry& registry);

	void destroy(ecs::ECSRegistry& registry);
}
