#pragma once

#include <memory>
#include <vector>

#include "CommandQueue.h"
#include "ComponentStorage.h"
#include "Resource.h"

namespace voxel_game::ecs {
	class ECSRegistry {
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

		template <typename T> requires std::derived_from<T, Component<T>> [[nodiscard]] T& getComponent(Entity entity) {
			const uint32_t id = T::getID();
			if (mComponentStorages.size() < id || !mComponentStorages[id]) {
				throw std::runtime_error("Entity does not have the requested component");
			}
			return reinterpret_cast<T*>(mComponentStorages[id].get())->get(entity);
		}

		template <typename T> requires std::derived_from<T, Component<T>> [[nodiscard]] bool hasComponent(Entity entity) {
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

		template <typename T> requires std::derived_from<T, Resource<T>> T& addResource(const T& resource) {
			const uint32_t id = T::getID();
			if (mResources.size() <= id) {
				mResources.resize(id + 1);
			}
			if (mResources[id]) {
				throw std::runtime_error("Resource already exists");
			}
			mResources[id] = std::make_unique<IResource>(resource);
			return reinterpret_cast<T&>(*mResources[id].get());
		}

		template <typename T> requires std::derived_from<T, Resource<T>> [[nodiscard]] T& getResource() {
			const uint32_t id = T::getID();
			if (mResources.size() < id) {
				mResources.resize(id + 1);
			}
			if (!mResources[id]) {
				throw std::runtime_error("Resource not loaded");
			}
			return reinterpret_cast<T&>(*mResources[id].get());
		}

		template <typename T> requires std::derived_from<T, Component<T>> [[nodiscard]] bool hasResource() {
			const uint32_t id = T::getID();
			if (mResources.size() < id || !mResources[id]) {
				return false;
			}
			return true;
		}

		template <typename T> requires std::derived_from<T, Resource<T>> void removeResource() {
			const uint32_t id = T::getID();
			if (mResources.size() < id || !mResources[id]) {
				mResources.resize(id + 1);
			}
			mResources[id]->destroy();
			mResources[id] = nullptr;
		}

		void pushCommand(const Command &command);

		void executeCommands();

	private:
		std::vector<std::unique_ptr<IComponentStorage>> mComponentStorages;
		std::vector<uint32_t> mFreeIDs;
		std::vector<std::unique_ptr<IResource>> mResources;
		CommandQueue mCommandQueue;
		uint32_t mNextID = 0;
	};
}