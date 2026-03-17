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
		PhysicsObject * getPhysicsObject(uint32_t id);

		std::vector<uint32_t> getPhysicsObjectsInSector(const glm::i64vec3& pos);

		void addPhysicsObject(PhysicsObject* object);

		void removePhysicsObject(uint32_t id);

	private:
		std::unordered_map<uint32_t, std::unique_ptr<PhysicsObject>> mObjects;
		std::unordered_map<glm::i64vec3, std::vector<uint32_t>, util::i64vec3Hasher> mSectorPhysicsObjects;
		uint32_t mNextID = 0;
	};
}
