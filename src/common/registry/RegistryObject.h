#pragma once

#include "RegistryID.h"

namespace voxel_game::registry {
	class RegistryObject {
	public:
		virtual ~RegistryObject() = default;

		[[nodiscard]] RegistryID getID() const;

		void onRegister(const RegistryID &id);

	private:
		RegistryID mID;
		bool mRegistered = false;
	};
}
