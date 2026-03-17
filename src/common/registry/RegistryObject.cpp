#include "RegistryObject.h"

#include <stdexcept>

namespace voxel_game::registry {
	RegistryID RegistryObject::getID() const {
		return mID;
	}

	void RegistryObject::onRegister(const RegistryID& id) {
		if (mRegistered) {
			throw std::runtime_error("Object already registered");
		}

		mID = id;
		mRegistered = true;
	}
}
