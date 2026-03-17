#include "Universe.h"

namespace voxel_game::universe {
	PhysicsObject * Universe::getPhysicsObject(const uint32_t id) {
		const auto it = mObjects.find(id);
		return it == mObjects.end()? nullptr: it->second.get();
	}

	std::vector<uint32_t> Universe::getPhysicsObjectsInSector(const glm::ivec3 &pos) {
		const auto it = mSectorPhysicsObjects.find(pos);
		return it == mSectorPhysicsObjects.end()? std::vector<uint32_t>{}: it->second;
	}

	void Universe::addPhysicsObject(PhysicsObject *object) {
		const uint32_t id = mNextID++;
		mObjects[id] = std::unique_ptr<PhysicsObject>(object);
		mSectorPhysicsObjects[object->getSectorPos()].push_back(id);
	}

	void Universe::removePhysicsObject(const uint32_t id) {
		const PhysicsObject *object = getPhysicsObject(id);
		mObjects.erase(id);
		std::vector<uint32_t> &physicsObjects = mSectorPhysicsObjects[object->getSectorPos()];
		for (uint32_t i = 0; i < physicsObjects.size(); i++) {
			if (physicsObjects[i] == id) {
				physicsObjects.erase(physicsObjects.begin() + i);
				return;
			}
		}
	}
}
