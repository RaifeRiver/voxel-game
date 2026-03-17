#pragma once

#include "glm/gtc/type_precision.hpp"

namespace voxel_game::universe {
	constexpr uint64_t SECTOR_SIZE = 64;

	class PhysicsObject {
	public:
		void move(glm::vec3 distance);

		void applyForce(glm::vec3 force, float deltaTime);

		void applyImpulse(glm::vec3 impulse);

		void update(float deltaTime);

		[[nodiscard]] glm::dvec3 getRelativePos(const PhysicsObject &other) const;

		[[nodiscard]] glm::i64vec3 getSectorPos() const;

		[[nodiscard]] glm::vec3 getLocalPos() const;

		[[nodiscard]] glm::quat getRotation() const;

		[[nodiscard]] glm::vec3 getVelocity() const;

	private:
		glm::i64vec3 mSectorPos;
		glm::vec3 mLocalPos;
		glm::quat mRotation;
		glm::vec3 mVelocity;

		void adjustLocalPos();
	};
}