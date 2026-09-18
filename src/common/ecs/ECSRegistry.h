/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "CommandQueue.h"
#include "ComponentStorage.h"
#include "Event.h"
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

		template <typename T> requires std::derived_from<T, Event<T>> void pushEvent(const T& event) {
			const uint32_t id = T::getID();
			if (mEvents[mCurrentEvents].size() < id) {
				mEvents[mCurrentEvents].resize(id + 1);
			}
			mEvents[mCurrentEvents][id].push_back(std::make_unique<T>(event));
		}

		template <typename T> requires std::derived_from<T, Event<T>> const std::vector<std::unique_ptr<T>>& getEvents() const {
			const uint32_t id = T::getID();
			if (mEvents[mOtherEvents].size() < id) {
				return {};
			}
			return reinterpret_cast<const std::vector<std::unique_ptr<T>>&>(mEvents[mOtherEvents][id]);
		}

		CommandQueue& getCommandQueue() {
			return mCommandQueue;
		}

		SystemManager& getSystemManager() {
			return mSystemManager;
		}

		void update(float deltaTime);

	private:
		std::vector<std::unique_ptr<IComponentStorage>> mComponentStorages;
		std::vector<uint32_t> mFreeIDs;
		std::vector<std::unique_ptr<IResource>> mResources;
		std::unordered_map<std::string, std::pair<uint32_t, ComponentProvider>> mComponentTypes;
		std::vector<std::vector<std::unique_ptr<IEvent>>> mEvents[2];
		uint32_t mCurrentEvents = 0;
		uint32_t mOtherEvents = 1;
		CommandQueue mCommandQueue;
		SystemManager mSystemManager;
		uint32_t mNextID = 0;
	};
}
