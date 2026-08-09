#pragma once

#include <memory>
#include <vector>

#include "ComponentStorage.h"

class Registry {
public:
	Entity createEntity();

	void destroyEntity(Entity entity);

	template <typename T> requires std::derived_from<T, Component<T>> T& attachComponent(Entity entity) {
		const uint32_t id = T::getID();
		if (mComponentStorages.size() < id) {
			mComponentStorages.resize(id + 1);
		}
		if (!mComponentStorages[id]) {
			mComponentStorages[id] = std::make_unique<ComponentStorage<T>>(entity);
		}
		return reinterpret_cast<T*>(mComponentStorages[id].get())->attach(entity);
	}

	template <typename T> requires std::derived_from<T, Component<T>> T& getComponent(Entity entity) {
		const uint32_t id = T::getID();
		if (mComponentStorages.size() < id || !mComponentStorages[id]) {
			throw std::runtime_error("Entity does not have the requested component");
		}
		return reinterpret_cast<T*>(mComponentStorages[id].get())->get(entity);
	}

	template <typename T> requires std::derived_from<T, Component<T>> bool hasComponent(Entity entity) {
		const uint32_t id = T::getID();
		if (mComponentStorages.size() < id || !mComponentStorages[id]) {
			return false;
		}
		return reinterpret_cast<T*>(mComponentStorages[id].get())->has(entity);
	}

	template <typename T> requires std::derived_from<T, Component<T>> void removeComponent(Entity entity) {
		const uint32_t id = T::getID();
		if (mComponentStorages.size() >= id && mComponentStorages[id]) {
			reinterpret_cast<T*>(mComponentStorages[id].get())->remove(entity);
		}
	}

private:
	std::vector<std::unique_ptr<IComponentStorage>> mComponentStorages;
	std::vector<uint32_t> mFreeIDs;
	uint32_t mNextID = 0;
};
