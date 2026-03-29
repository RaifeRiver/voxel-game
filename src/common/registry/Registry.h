#pragma once

#include <memory>
#include <unordered_map>

#include "RegistryObject.h"

namespace voxel_game::registry {
	template<std::derived_from<RegistryObject> T> class Registry {
	public:
		T* registerObject(const RegistryID& id, std::unique_ptr<T> object) {
			if (mLocked) {
				throw std::runtime_error("Registry is locked");
			}

			mObjects[id] = std::move(object);
			return mObjects[id].get();
		}

		[[nodiscard]] T* getObject(const RegistryID& id) {
			return mObjects[id].get();
		}

		void lock() {
			if (mLocked) {
				throw std::runtime_error("Registry already locked");
			}

			mLocked = true;
		}

	private:
		std::unordered_map<RegistryID, std::unique_ptr<T>, RegistryIDHash> mObjects;
		bool mLocked = false;
	};
}
