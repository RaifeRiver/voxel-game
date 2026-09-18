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

#include "UniverseLoader.h"

#include <ranges>

#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#include "UniverseLoaderInfo.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadSectorEvent.h"
#include "common/event/UnloadSectorEvent.h"
#include "common/util/MathUtil.h"

namespace voxel_game::universe {
	void UniverseLoader::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		ZoneScopedN("Load universe");

		const std::vector<ecs::Entity> entities = registry.getEntitiesWithComponents<UniverseLoaderInfo, component::Transform>();

		for (const ecs::Entity entity : entities) {
			UniverseLoaderInfo& universeLoaderInfo = registry.getComponent<UniverseLoaderInfo>(entity);
			const component::Transform& transform = registry.getComponent<component::Transform>(entity);

			if (universeLoaderInfo.hasLastSector && universeLoaderInfo.lastSector == transform.pos.sector) {
				continue;
			}

			const int32_t radius = universeLoaderInfo.radius;

			std::vector<UniverseLoaderInfo> intersectingEntities;
			for (const ecs::Entity otherEntity : entities) {
				if (otherEntity == entity) {
					continue;
				}
				const UniverseLoaderInfo& otherLoaderInfo = registry.getComponent<UniverseLoaderInfo>(otherEntity);
				if (!otherLoaderInfo.hasLastSector) {
					continue;
				}
				const component::Transform& otherTransform = registry.getComponent<component::Transform>(otherEntity);
				if (util::checkSpheresIntersect<int64_t>(transform.pos.sector, universeLoaderInfo.radius, otherTransform.pos.sector, otherLoaderInfo.radius)) {
					intersectingEntities.push_back(otherLoaderInfo);
				}
			}

			if (universeLoaderInfo.hasLastSector) {
				const int32_t radius2 = radius * radius;
				for (int32_t x = -radius; x <= radius; x++) {
					const int32_t x2 = x * x;
					const int32_t maxY = std::sqrt(radius2 - x2);
					for (int32_t y = -maxY; y <= maxY; y++) {
						const int32_t maxZ = std::sqrt(radius2 - x2 - y * y);
						for (int32_t z = -maxZ; z <= maxZ; z++) {
							glm::i64vec3 newSector = transform.pos.sector + glm::i64vec3{x, y, z};
							const bool intersectsOld = util::checkPointIntersectsSphere<int64_t>(universeLoaderInfo.lastSector, universeLoaderInfo.radius, newSector);
							const glm::i64vec3 oldSector = universeLoaderInfo.lastSector + glm::i64vec3{x, y, z};
							const bool intersectsNew = util::checkPointIntersectsSphere<int64_t>(transform.pos.sector, universeLoaderInfo.radius, oldSector);
							if (!intersectsOld || !intersectsNew) {
								bool otherEntityLoadingNew = false;
								for (const UniverseLoaderInfo& otherLoadInfo: intersectingEntities) {
									if (util::checkPointIntersectsSphere<int64_t>(otherLoadInfo.lastSector, otherLoadInfo.radius, newSector)) {
										otherEntityLoadingNew = true;
										break;
									}
								}
								if (!otherEntityLoadingNew && !intersectsOld) {
									loadSector(registry, newSector);
								}
								bool otherEntityLoadingOld = false;
								for (const UniverseLoaderInfo& otherLoadInfo: intersectingEntities) {
									if (util::checkPointIntersectsSphere<int64_t>(otherLoadInfo.lastSector, otherLoadInfo.radius, oldSector)) {
										otherEntityLoadingOld = true;
										break;
									}
								}
								if (!otherEntityLoadingOld && !intersectsNew) {
									unloadSector(registry, oldSector);
								}
							}
						}
					}
				}
			}
			else {
				const int32_t radius2 = radius * radius;
				for (int32_t x = -radius; x <= radius; x++) {
					const int32_t x2 = x * x;
					const int32_t maxY = std::sqrt(radius2 - x2);
					for (int32_t y = -maxY; y <= maxY; y++) {
						const int32_t maxZ = std::sqrt(radius2 - x2 - y * y);
						for (int32_t z = -maxZ; z <= maxZ; z++) {
							glm::i64vec3 sector = transform.pos.sector + glm::i64vec3{x, y, z};
							bool loaded = false;
							for (const UniverseLoaderInfo& otherLoadInfo: intersectingEntities) {
								if (util::checkPointIntersectsSphere(otherLoadInfo.lastSector, static_cast<int64_t>(otherLoadInfo.radius), sector)) {
									loaded = true;
									break;
								}
							}
							if (!loaded) {
								loadSector(registry, sector);
							}
						}
					}
				}
				universeLoaderInfo.hasLastSector = true;
			}

			universeLoaderInfo.lastSector = transform.pos.sector;

			std::cout << "Loaded sectors: " << mLoadedSectors << std::endl;
		}
	}

	void UniverseLoader::loadSector(ecs::ECSRegistry& registry, const glm::i64vec3& sector) {
		mLoadedSectors++;

		registry.pushEvent<event::LoadSectorEvent>(sector);
	}

	void UniverseLoader::unloadSector(ecs::ECSRegistry& registry, const glm::i64vec3& sector) {
		mLoadedSectors--;

		registry.pushEvent<event::UnloadSectorEvent>(sector);
	}
}
