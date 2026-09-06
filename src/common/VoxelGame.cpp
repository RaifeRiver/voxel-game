#include "VoxelGame.h"

#include "tracy/Tracy.hpp"

#include "block/BlockLoader.h"
#include "component/Components.h"
#include "resource/ResourceManager.h"

namespace voxel_game {
	void preLoad(ecs::ECSRegistry& registry) {
		ZoneScopedN("Pre init common");

		component::registerComponents(registry);

		registry.createResource<resource::ResourceManager>();
	}

	void load(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init common");

		block::loadBlocks(registry);
	}

	void postLoad(ecs::ECSRegistry& registry) {
		ZoneScopedN("Post init common");
	}

	void destroy(ecs::ECSRegistry& registry) {

	}
}
