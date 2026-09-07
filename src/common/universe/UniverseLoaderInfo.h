#pragma once

#include "UniversePos.h"
#include "common/ecs/Component.h"

namespace voxel_game::universe {
	struct UniverseLoaderInfo : ecs::Component<UniverseLoaderInfo> {
		int32_t radius = 0.0f;

	private:
		bool hasLastSector = false;
		glm::i64vec3 lastSector;

		friend class UniverseLoader;
	};
}
