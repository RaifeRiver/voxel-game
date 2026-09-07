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

#include "SystemManager.h"

#include "ECSRegistry.h"

namespace voxel_game::ecs {
	void SystemManager::registerSystem(SystemStage stage, const SystemFunction& system) {
		mStages[static_cast<size_t>(stage)].push_back(system);
	}

	void SystemManager::runSystems(ECSRegistry& registry, const float deltaTime) {
		for (size_t i = 0; i < static_cast<size_t>(SystemStage::COUNT); i++) {
			for (SystemFunction& system : mStages[i]) {
				system(registry, deltaTime);
			}
			for (const std::unique_ptr<ISystem>& system: mSystems) {
				system->runStage(static_cast<SystemStage>(i), registry, deltaTime);
			}
		}
	}

	void SystemManager::removeSystem(const uint32_t id) {
		for (size_t i = 0; i < mSystems.size(); i++) {
			if (mSystems[i]->getID() == id) {
				mSystems.erase(mSystems.begin() + static_cast<uint32_t>(i));
				break;
			}
		}
	}
}
