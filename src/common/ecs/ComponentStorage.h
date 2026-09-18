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

#include <algorithm>
#include <concepts>
#include <stdexcept>
#include <vector>

#include "Component.h"
#include "Entity.h"

namespace voxel_game::ecs {
	class IComponentStorage {
	public:
		virtual void remove(Entity entity) = 0;

		virtual IComponent& attach_(Entity entity) = 0;

		virtual ~IComponentStorage() = default;
	};

	template <typename T> requires std::derived_from<T, Component<T>> class ComponentStorage : public IComponentStorage {
	public:
		IComponent& attach_(const Entity entity) override {
			if (!has(entity)) {
				if (entity >= mComponentIndices.size()) {
					mComponentIndices.resize(entity + 1);
				}
				if (!mUnusedComponents.empty()) {
					uint32_t index = mUnusedComponents.back();
					mComponentIndices[entity] = index;
					mUnusedComponents.pop_back();
					mComponents[index] = {};
					return mComponents[index];
				}
				const uint32_t index = mComponents.size();
				mComponentIndices[entity] = index;
				mComponents.push_back({});
				return mComponents.back();
			}
			return get(entity);
		}

		T& attach(const Entity entity, T component = T{}) {
			if (has(entity)) {
				return mComponents[mComponentIndices[entity]];
			}
			if (entity >= mComponentIndices.size()) {
				mComponentIndices.resize(entity + 1, UINT32_MAX);
			}
			if (!mUnusedComponents.empty()) {
				uint32_t index = mUnusedComponents.back();
				mComponentIndices[entity] = index;
				mUnusedComponents.pop_back();
				mComponents[index] = std::move(component);
				return mComponents[index];
			}
			uint32_t index = mComponents.size();
			mComponentIndices[entity] = index;
			mComponents.push_back(std::move(component));
			return mComponents[index];
		}

		[[nodiscard]] T& get(const Entity entity) {
			if (!has(entity)) {
				throw std::runtime_error("Entity does not have the requested component");
			}
			return mComponents[mComponentIndices[entity]];
		}

		[[nodiscard]] bool has(const Entity entity) const {
			return mComponentIndices.size() > entity && mComponentIndices[entity] != UINT32_MAX;
		}

		void remove(const Entity entity) override {
			if (!has(entity)) {
				return;
			}
			size_t index = mComponentIndices[entity];
			mComponents[index] = {};
			mUnusedComponents.push_back(index);
			mComponentIndices[entity] = UINT32_MAX;
		}

		class Iterator {
		public:
			explicit Iterator(const ComponentStorage& componentStorage, const size_t current) : mComponentStorage(componentStorage), mCurrent(current) {
				if (std::ranges::find(mComponentStorage.mUnusedComponents, mCurrent) != mComponentStorage.mUnusedComponents.end()) {
					while (std::ranges::find(mComponentStorage.mUnusedComponents, ++mCurrent) != mComponentStorage.mUnusedComponents.end()) {}
				}
			}

			Entity operator*() const {
				return Entity(mCurrent);
			}

			Iterator& operator++() {
				while (std::ranges::find(mComponentStorage.mUnusedComponents, ++mCurrent) != mComponentStorage.mUnusedComponents.end()) {}
				return *this;
			}

			bool operator!=(const Iterator& other) const {
				return mCurrent != other.mCurrent;
			}

		private:
			const ComponentStorage& mComponentStorage;
			size_t mCurrent;
		};

		Iterator begin() {
			return Iterator(*this, 0);
		}

		Iterator end() {
			size_t current = mComponents.size();
			while (std::ranges::find(mUnusedComponents, --current) != mUnusedComponents.end()) {}
			return Iterator(*this, current);
		}

	private:
		std::vector<uint32_t> mComponentIndices;
		std::vector<T> mComponents;
		std::vector<uint32_t> mUnusedComponents;
	};
}