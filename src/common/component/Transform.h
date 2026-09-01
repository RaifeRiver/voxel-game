#pragma once

#include "glm/ext.hpp"

#include "common/ecs/Component.h"
#include "common/universe/UniversePos.h"

namespace voxel_game::component {
	struct Transform : ecs::Component<Transform> {
		universe::UniversePos pos;
		glm::quat rotation;
		glm::vec3 scale;
	};
}
