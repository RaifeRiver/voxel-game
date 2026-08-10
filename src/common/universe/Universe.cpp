#include "Universe.h"

namespace voxel_game::universe {
	void Universe::addObject(const ecs::Entity object) {
		mObjects.push_back(object);
	}

	void Universe::removeObject(const ecs::Entity object) {
		std::erase(mObjects, object);
	}

	std::vector<ecs::Entity>& Universe::getObjects() {
		return mObjects;
	}

	void Universe::clearObjects() {
		mObjects.clear();
	}
}
