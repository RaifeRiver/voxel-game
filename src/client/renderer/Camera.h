#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace voxel_game::client::renderer {
	class Camera {
	public:
		glm::vec3 velocity;
		glm::vec3 position;

		float pitch;
		float yaw;

		[[nodiscard]] glm::mat4 getViewMatrix() const;

		[[nodiscard]] glm::mat4 getRotationMatrix() const;

		void processMouseInput(double x, double y);

		void processKeyInput(int key, int action);

		void update(float deltaTime);
	};
}
