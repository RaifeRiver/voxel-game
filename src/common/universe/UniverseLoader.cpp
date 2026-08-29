#include "UniverseLoader.h"

#include <ranges>

#include "UniverseLoaderInfo.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::universe {
	void UniverseLoader::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		const std::vector<ecs::Entity> entities = registry.getEntitiesWithComponents<UniverseLoaderInfo, Transform>();

		bool needsUpdate = false;
		for (const ecs::Entity entity : entities) {
			const auto& universeLoaderInfo = registry.getComponent<UniverseLoaderInfo>(entity);
			const auto& transform = registry.getComponent<Transform>(entity);

			const double distance = (transform.pos - universeLoaderInfo.lastPos).length();
			if (distance >= LOAD_DISTANCE_THRESHOLD || !universeLoaderInfo.hasLastPos) {
				needsUpdate = true;
				break;
			}
		}
		if (!needsUpdate) {
			return;
		}

		for (uint64_t& loaded: mLoadedSectors | std::views::values) {
			loaded = 0;
		}

		for (const ecs::Entity entity : entities) {
			auto& universeLoaderInfo = registry.getComponent<UniverseLoaderInfo>(entity);
			const auto& transform = registry.getComponent<Transform>(entity);

			const int32_t radius = universeLoaderInfo.radius;
			const int64_t radiusSquared = radius * radius;
			for (int32_t x = -radius; x < radius; x++) {
				for (int32_t y = -radius; y < radius; y++) {
					for (int32_t z = -radius; z < radius; z++) {
						const int64_t distanceSquared = x * x + y * y + z * z;
						if (distanceSquared > radiusSquared) {
							continue;
						}
						glm::i64vec3 sector = transform.pos.sector + glm::i64vec3{x, y, z};
						if (!mLoadedSectors.contains(sector)) {
							loadSector(sector);
						}
					}
				}
			}

			universeLoaderInfo.lastPos = transform.pos;
			universeLoaderInfo.hasLastPos = true;
		}

		uint64_t loadedSectors = 0;
		for (const auto& [index, loaded]: mLoadedSectors) {
			for (int64_t i = 0; i < 64; i++) {
				if (!(loaded & (1 << i))) {
					glm::i64vec3 sector = index;
					sector.z <<= 6;
					sector.z |= i;
					unloadSector(sector);
				}
				else {
					loadedSectors++;
				}
			}
		}
	}

	void UniverseLoader::loadSector(const glm::i64vec3& sector) {
		glm::i64vec3 mapIndex = sector;
		mapIndex.z >>= 6;
		mLoadedSectors[mapIndex] |= 1 << (sector.z & 63);
	}

	void UniverseLoader::unloadSector(const glm::i64vec3& sector) {
		glm::i64vec3 mapIndex = sector;
		mapIndex.z >>= 6;
		mLoadedSectors[mapIndex] &= ~(1 << (sector.z & 63));
	}
}
