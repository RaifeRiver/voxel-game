#include "VoxelGame.h"

#include "resource/ResourceManager.h"

namespace voxel_game {
	void preLoad(ecs::ECSRegistry& registry) {
		registry.createResource<resource::ResourceManager>();
	}

	void load(ecs::ECSRegistry& registry) {

	}

	void destroy(ecs::ECSRegistry& registry) {

	}
}
