#pragma once

#include "CommandLineArguments.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry, const CommandLineArguments& args);

	void run(ecs::ECSRegistry& registry);

	void destroy(ecs::ECSRegistry& registry);
}
