#include "ECSRegistry.h"

namespace voxel_game::ecs {
	Entity ECSRegistry::createEntity() {
		if (!mFreeIDs.empty()) {
			const uint32_t id = mFreeIDs.back();
			mFreeIDs.pop_back();
			return Entity(id);
		}
		return Entity(mNextID++);
	}

	void ECSRegistry::destroyEntity(const Entity entity) {
		mFreeIDs.push_back(entity);
	}

	void ECSRegistry::pushCommand(const Command &command) {
		mCommandQueue.pushCommand(command);
	}

	void ECSRegistry::executeCommands() {
		mCommandQueue.execute(*this);
	}
}