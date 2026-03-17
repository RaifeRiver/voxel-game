#include "RegistryID.h"

namespace voxel_game::registry {
	std::string RegistryID::getID() const {
		return namespace_ + ":" + id;
	}

	bool RegistryID::operator==(const RegistryID &other) const {
		return namespace_ == other.namespace_ && id == other.id;
	}
}
