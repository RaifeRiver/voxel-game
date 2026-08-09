#include "Registry.h"

Entity Registry::createEntity() {
	if (!mFreeIDs.empty()) {
		const uint32_t id = mFreeIDs.back();
		mFreeIDs.pop_back();
		return Entity(id);
	}
	return Entity(mNextID++);
}

void Registry::destroyEntity(const Entity entity) {
	mFreeIDs.push_back(entity);
}

void Registry::pushCommand(const Command &command) {
	mCommandQueue.pushCommand(command);
}

void Registry::executeCommands() {
	mCommandQueue.execute(*this);
}
