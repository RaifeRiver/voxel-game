#pragma once

#include "glm/vec3.hpp"

#include "common/ecs/Component.h"

namespace voxel_game::component {
	struct Velocity : ecs::Component<Velocity> {
		glm::vec3 velocity;

		[[nodiscard]] operator glm::vec3() const {
			return velocity;
		}

		void operator*=(const float f) {
			velocity *= f;
		}

		void operator+=(const glm::vec3 v) {
			velocity += v;
		}

		Velocity operator*(const float deltaTime) const {
			Velocity result;
			result.velocity = velocity * deltaTime;
			return result;
		}
	};
}
