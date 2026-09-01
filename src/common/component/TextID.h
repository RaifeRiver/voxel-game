#pragma once

#include <string>

#include "common/ecs/Component.h"

namespace voxel_game::component {
	struct TextID : ecs::Component<TextID> {
		std::string namespace_;
		std::string name;
	};
}
