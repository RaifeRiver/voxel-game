#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include "PhysicsObject.h"
#include "common/util/Hashers.h"

namespace voxel_game::universe {
	class Universe {
	public:
		PhysicsObject * getPhysicsObject(uint64_t id);

		std::vector<uint64_t> getPhysicsObjectsInSector(const glm::i64vec3& pos);

		void addPhysicsObject(PhysicsObject* object);

		void removePhysicsObject(uint64_t id);

	private:
		std::unordered_map<uint64_t, std::unique_ptr<PhysicsObject>> mObjects;
		std::unordered_map<glm::i64vec3, std::vector<uint64_t>, util::vec3Hasher<int64_t, glm::i64vec3>> mSectorPhysicsObjects;
		uint64_t mNextID = 0;
	};
}
