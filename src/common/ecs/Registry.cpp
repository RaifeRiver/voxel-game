#include "Registry.h"

Entity Registry::createEntity() {
	if (mFreeIDs.size() > 0) {
		const uint32_t id = mFreeIDs.back();
		mFreeIDs.pop_back();
		return Entity(id);
	}
	return Entity(mNextID++);
}

void Registry::destroyEntity(const Entity entity) {
	mFreeIDs.push_back(entity);
}
