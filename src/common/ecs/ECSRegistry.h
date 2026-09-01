#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "CommandQueue.h"
#include "ComponentStorage.h"
#include "Resource.h"
#include "SystemManager.h"

namespace voxel_game::ecs {
	using ComponentProvider = std::function<std::unique_ptr<IComponent>()>;

	class ECSRegistry {
	public:
		Entity createEntity();

		void destroyEntity(Entity entity);

		template <std::derived_from<IComponent>... T> std::vector<Entity> getEntitiesWithComponents() {
			std::vector<Entity> entities;
			for (uint32_t i = 0; i < mNextID; i++) {
				if (std::ranges::find(mFreeIDs, i) != mFreeIDs.end()) {
					continue;
				}
				const auto entity = Entity(i);
				if ((hasComponent<T>(entity) && ...)) {
					entities.push_back(entity);
				}
			}
			return entities;
		}

		template <typename T> requires std::derived_from<T, Component<T>> void registerComponentType(const std::string& name) {
			mComponentTypes[name] = {T::getID(), [] {
				return std::make_unique<T>();
			}};
		}

		template <typename T> requires std::derived_from<T, Component<T>> T& attachComponent(Entity entity, T component = {}) {
			const uint32_t id = T::getID();
			if (mComponentStorages.size() <= id) {
				mComponentStorages.resize(id + 1);
			}
			if (!mComponentStorages[id]) {
				mComponentStorages[id] = std::make_unique<ComponentStorage<T>>();
			}
			return reinterpret_cast<ComponentStorage<T>*>(mComponentStorages[id].get())->attach(entity, component);
		}

		IComponent& attachComponent(const Entity entity, const std::string& name) {
			auto [id, provider] = mComponentTypes[name];
			if (mComponentStorages.size() <= id) {
				mComponentStorages.resize(id + 1);
			}
			return mComponentStorages[id].get()->attach_(entity);
		}

		template <typename T> requires std::derived_from<T, Component<T>> [[nodiscard]] T& getComponent(Entity entity) {
			const uint32_t id = T::getID();
			if (mComponentStorages.size() <= id || !mComponentStorages[id]) {
				throw std::runtime_error("Entity does not have the requested component");
			}
			return reinterpret_cast<ComponentStorage<T>*>(mComponentStorages[id].get())->get(entity);
		}

		template <typename T> requires std::derived_from<T, Component<T>> [[nodiscard]] bool hasComponent(Entity entity) {
			const uint32_t id = T::getID();
			if (mComponentStorages.size() <= id || !mComponentStorages[id]) {
				return false;
			}
			return reinterpret_cast<ComponentStorage<T>*>(mComponentStorages[id].get())->has(entity);
		}

		template <typename T> requires std::derived_from<T, Component<T>> void removeComponent(Entity entity) {
			const uint32_t id = T::getID();
			if (mComponentStorages.size() > id && mComponentStorages[id]) {
				reinterpret_cast<ComponentStorage<T>*>(mComponentStorages[id].get())->remove(entity);
			}
		}

		template <typename T, typename U = T> requires std::derived_from<T, Resource<T>> && std::derived_from<U, T> T& addResource(const U& resource) {
			const uint32_t id = T::getID();
			if (mResources.size() <= id) {
				mResources.resize(id + 1);
			}
			if (mResources[id]) {
				throw std::runtime_error("Resource already exists");
			}
			mResources[id] = std::make_unique<U>(std::move(resource));
			return dynamic_cast<T&>(*mResources[id].get());
		}

		template <typename T, typename U = T, typename... Args> requires std::derived_from<T, Resource<T>> && std::derived_from<U, T> T& createResource(Args&&... args) {
			const uint32_t id = T::getID();
			if (mResources.size() <= id) {
				mResources.resize(id + 1);
			}
			if (mResources[id]) {
				throw std::runtime_error("Resource already exists");
			}
			mResources[id] = std::make_unique<U>(std::forward<Args>(args)...);
			return dynamic_cast<T&>(*mResources[id].get());
		}

		template <typename T> requires std::derived_from<T, Resource<T>> [[nodiscard]] T& getResource() {
			const uint32_t id = T::getID();
			if (mResources.size() <= id) {
				mResources.resize(id + 1);
			}
			if (!mResources[id]) {
				throw std::runtime_error("Resource not loaded");
			}
			return dynamic_cast<T&>(*mResources[id].get());
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

		CommandQueue& getCommandQueue() {
			return mCommandQueue;
		}

		SystemManager& getSystemManager() {
			return mSystemManager;
		}

	private:
		std::vector<std::unique_ptr<IComponentStorage>> mComponentStorages;
		std::vector<uint32_t> mFreeIDs;
		std::vector<std::unique_ptr<IResource>> mResources;
		std::unordered_map<std::string, std::pair<uint32_t, ComponentProvider>> mComponentTypes;
		CommandQueue mCommandQueue;
		SystemManager mSystemManager;
		uint32_t mNextID = 0;
	};
}
