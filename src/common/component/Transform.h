#pragma once

#include "glm/ext.hpp"

#include "common/ecs/Component.h"
#include "common/universe/UniversePos.h"

namespace voxel_game::universe {
	struct Transform : ecs::Component<Transform> {
		UniversePos pos;
		glm::quat rotation;
		glm::vec3 scale;
	};
}
