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

#include <cstdint>
#include <functional>
#include <memory>

#include "System.h"

namespace voxel_game::ecs {
	class ECSRegistry;

	class SystemManager {
	public:
		void registerSystem(SystemStage stage, const SystemFunction& system);

		template <typename T> requires std::derived_from<T, System<T>> void registerSystem(const T& system) {
			const uint32_t id = system.getID();
			if (id >= mSystems.size()) {
				mSystems.resize(id + 1);
			}
			mSystems[id] = std::unique_ptr<T>(*system);
		}

		template <typename T, typename... Args> requires std::derived_from<T, System<T>> void createSystem(Args&&... args) {
			const uint32_t id = T::getID_();
			if (id >= mSystems.size()) {
				mSystems.resize(id + 1);
			}
			mSystems[id] = std::make_unique<T>(std::forward<Args>(args)...);
		}

		void runSystems(ECSRegistry& registry, float deltaTime);

		void removeSystem(uint32_t id);

		template <typename T> requires std::derived_from<T, System<T>> void removeSystem() {
			const uint32_t id = T::getID_();
			mSystems[id] = nullptr;
		}

	private:
		std::vector<SystemFunction> mStages[static_cast<size_t>(SystemStage::COUNT)];
		std::vector<std::unique_ptr<ISystem>> mSystems;
	};
}
