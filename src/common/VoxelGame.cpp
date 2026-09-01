#include "VoxelGame.h"

#include "block/BlockLoader.h"
#include "component/Components.h"
#include "resource/ResourceManager.h"
#include "universe/UniverseLoader.h"

namespace voxel_game {
	void preLoad(ecs::ECSRegistry& registry) {
		component::registerComponents(registry);

		registry.createResource<resource::ResourceManager>();
	}

	void load(ecs::ECSRegistry& registry) {
		block::loadBlocks(registry);
	}

	void postLoad(ecs::ECSRegistry& registry) {
		registry.getSystemManager().createSystem<universe::UniverseLoader>();
	}

	void destroy(ecs::ECSRegistry& registry) {

	}
}
