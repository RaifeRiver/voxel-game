#include "PhysicsObject.h"

namespace voxel_game::universe {
	void PhysicsObject::move(const glm::vec3 distance) {
		mLocalPos += distance;

		adjustLocalPos();
	}

	void PhysicsObject::applyForce(const glm::vec3 force, const float deltaTime) {
		mVelocity += force * deltaTime;
	}

	void PhysicsObject::applyImpulse(const glm::vec3 impulse) {
		mVelocity += impulse;
	}

	void PhysicsObject::update(const float deltaTime) {
		move(mVelocity * deltaTime);

		mVelocity *= 0.95f;
	}

	glm::dvec3 PhysicsObject::getRelativePos(const PhysicsObject &other) const {
		return glm::dvec3((mSectorPos - other.mSectorPos) * 64L) + glm::dvec3(mLocalPos) - glm::dvec3(other.mLocalPos);
	}

	glm::i64vec3 PhysicsObject::getSectorPos() const {
		return mSectorPos;
	}

	glm::vec3 PhysicsObject::getLocalPos() const {
		return mLocalPos;
	}

	glm::quat PhysicsObject::getRotation() const {
		return mRotation;
	}

	glm::vec3 PhysicsObject::getVelocity() const {
		return mVelocity;
	}

	void PhysicsObject::adjustLocalPos() {
		for (int i = 0; i < 3; i++) {
			const uint32_t sectors = std::floor(mLocalPos[i] / SECTOR_SIZE);
			if (sectors != 0) {
				mLocalPos[i] = mLocalPos[i] - SECTOR_SIZE * sectors;
				mSectorPos[i] += sectors;
			}
		}
	}
}
