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

#include "ECSRegistry.h"

namespace voxel_game::ecs {
	Entity ECSRegistry::createEntity() {
		if (!mFreeIDs.empty()) {
			const uint32_t id = mFreeIDs.back();
			mFreeIDs.pop_back();
			return Entity(id);
		}
		return Entity(mNextID++);
	}

	void ECSRegistry::destroyEntity(const Entity entity) {
		mFreeIDs.push_back(entity);
		for (const std::unique_ptr<IComponentStorage>& componentStorage: mComponentStorages) {
			if (componentStorage) {
				componentStorage->remove(entity);
			}
		}
	}

	void ECSRegistry::update(const float deltaTime) {
		mCommandQueue.execute(*this);
		mSystemManager.runSystems(*this, deltaTime);

		for (std::vector<std::unique_ptr<IEvent>>& events : mEvents[mOtherEvents]) {
			events.clear();
		}
		std::swap(mCurrentEvents, mOtherEvents);
	}
}
