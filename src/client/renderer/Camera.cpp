#include "Camera.h"

#include "GLFW/glfw3.h"
#include "glm/detail/type_quat.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

namespace voxel_game::client::renderer {
	glm::mat4 Camera::getViewMatrix() const {
		const glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
		const glm::mat4 rotation = getRotationMatrix();
		return glm::inverse(translation * rotation);
	}

	glm::mat4 Camera::getRotationMatrix() const {
		const glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
	}

	void Camera::processMouseInput(const double x, const double y) {
		yaw += static_cast<float>(x) / 500.0f;
		pitch += -static_cast<float>(y) / 500.0f;

		if (pitch < -1.5f) {
			pitch = -1.5f;
		}
		else if (pitch > 1.5f) {
			pitch = 1.5f;
		}
	}

	void Camera::processKeyInput(const int key, const int action) {
		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_W) {
				velocity.z = -0.2f;
			}
			else if (key == GLFW_KEY_S) {
				velocity.z = 0.2f;
			}
			else if (key == GLFW_KEY_A) {
				velocity.x = -0.2f;
			}
			else if (key == GLFW_KEY_D) {
				velocity.x = 0.2f;
			}
			else if (key == GLFW_KEY_SPACE) {
				velocity.y = 0.2f;
			}
			else if (key == GLFW_KEY_LEFT_SHIFT) {
				velocity.y = -0.2f;
			}
		}
		else if (action == GLFW_RELEASE) {
			if (key == GLFW_KEY_W || key == GLFW_KEY_S) {
				velocity.z = 0.0f;
			}
			else if (key == GLFW_KEY_A || key == GLFW_KEY_D) {
				velocity.x = 0.0f;
			}
			else if (key == GLFW_KEY_SPACE || key == GLFW_KEY_LEFT_SHIFT) {
				velocity.y = 0.0f;
			}
		}
	}

	void Camera::update(const float deltaTime) {
		position += glm::vec3(glm::toMat4(glm::angleAxis(yaw, glm::vec3(0.0f, -1.0f, 0.0f))) * glm::vec4(velocity.x, 0.0f, velocity.z, 0.0f)) * 200.0f * deltaTime;
		position.y += velocity.y * 200.0f * deltaTime;
	}
}
