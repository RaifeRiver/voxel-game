#pragma once

#include <vector>

#include "common/ecs/Entity.h"
#include "common/ecs/Resource.h"

namespace voxel_game::ecs {
	class Entity;
}

namespace voxel_game::universe {
	class Universe : public ecs::Resource<Universe> {
	public:
		void addObject(ecs::Entity object);

		void removeObject(ecs::Entity object);

		std::vector<ecs::Entity>& getObjects();

		void clearObjects();

	private:
		std::vector<ecs::Entity> mObjects;
	};
}
