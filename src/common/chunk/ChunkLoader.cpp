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

#include "ChunkLoader.h"

#include "tracy/Tracy.hpp"

#include "ChunkData.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadChunkEvent.h"
#include "common/event/UnloadChunkEvent.h"
#include "common/universe/UniversePos.h"
#include "common/util/UpdateTime.h"

namespace voxel_game::chunk {
	constexpr uint32_t MAX_CHUNKS_PER_UPDATE = 100;

	void ChunkLoader::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		ZoneScopedN("Loading chunks");

		util::UpdateTime& updateTime = registry.getResource<util::UpdateTime>();

		const std::vector<ecs::Entity> chunkLoaders = registry.getEntitiesWithComponents<ChunkLoaderInfo, component::Transform>();
		const std::vector<ecs::Entity> chunkObjects = registry.getEntitiesWithComponents<ChunkData, component::Transform>();

		for (const ecs::Entity chunkObject : chunkObjects) {
			ChunkData& chunkData = registry.getComponent<ChunkData>(chunkObject);
			component::Transform& objectTransform = registry.getComponent<component::Transform>(chunkObject);

			for (const ecs::Entity chunkLoader : chunkLoaders) {
				const ChunkLoaderInfo& chunkLoaderInfo = registry.getComponent<ChunkLoaderInfo>(chunkLoader);
				component::Transform& loaderTransform = registry.getComponent<component::Transform>(chunkLoader);

				const float loadRadius = static_cast<float>(chunkLoaderInfo.radius) * CHUNK_SIZE;
				const float loadRadius2 = loadRadius * loadRadius;

				glm::vec3 sphereCentre = glm::conjugate(objectTransform.rotation) * glm::vec3(loaderTransform.pos - objectTransform.pos);

				const glm::vec3 minBound = sphereCentre - loadRadius;
				const glm::vec3 maxBound = sphereCentre + loadRadius;

				const auto minChunk = glm::ivec3(glm::floor(minBound / static_cast<float>(CHUNK_SIZE)));
				const auto maxChunk = glm::ivec3(glm::floor(maxBound / static_cast<float>(CHUNK_SIZE)));

				std::vector<std::pair<float, glm::ivec3>> chunksToLoad;

				for (int32_t x = minChunk.x; x <= maxChunk.x; x++) {
					for (int32_t y = minChunk.y; y <= maxChunk.y; y++) {
						for (int32_t z = minChunk.z; z <= maxChunk.z; z++) {
							glm::ivec3 chunk = {x, y, z};

							glm::i64vec3 chunkCentre = glm::i64vec3(chunk) * static_cast<int64_t>(CHUNK_SIZE) + static_cast<int64_t>(HALF_CHUNK_SIZE);
							auto distanceVec = glm::dvec3(chunkCentre) - glm::dvec3(sphereCentre);
							double distance = glm::dot(distanceVec, distanceVec);
							if (distance <= loadRadius2) {
								if (!chunkData.isLoaded(chunk)) {
									chunksToLoad.emplace_back(distance, chunk);
								}
								else {
									chunkData.chunks.at(chunk).setLastLoaded(updateTime.update);
								}
							}
						}
					}
				}

				std::ranges::sort(chunksToLoad, [](const std::pair<float, glm::ivec3>& a, const std::pair<float, glm::ivec3>& b) {
					return a.first < b.first;
				});

				for (uint32_t i = 0; i < std::min(static_cast<uint32_t>(chunksToLoad.size()), MAX_CHUNKS_PER_UPDATE); i++) {
					glm::ivec3 pos = chunksToLoad[i].second;
					chunkData.chunks.emplace(pos, createChunk(pos, chunkObject));
					Chunk& chunk = chunkData.chunks.at(pos);
					chunk.setLastLoaded(updateTime.update);
					registry.pushEvent<event::LoadChunkEvent>({chunkObject, chunk});
				}
			}

			std::erase_if(chunkData.chunks, [&](const auto& chunk) {
				if (chunk.second.getLastLoaded() < updateTime.update) {
					registry.pushEvent<event::UnloadChunkEvent>({chunkObject, chunk.first});
					return true;
				}
				return false;
			});
		}
	}

	Chunk ChunkLoader::createChunk(const glm::ivec3 pos, const ecs::Entity entity) {
		ZoneScopedN("Creating chunk");

		Chunk chunk = {pos, entity};

		if (pos.y == 0) {
			for (uint32_t x = 0; x < CHUNK_SIZE; x++) {
				for (uint32_t z = 0; z < CHUNK_SIZE; z++) {
					const uint32_t height = static_cast<uint32_t>((static_cast<double>(std::rand()) / RAND_MAX * 0.2 + 0.6) * CHUNK_SIZE);
					for (uint32_t y = 0; y < height; y++) {
						chunk.setBlock(x, y, z, z * 32 * 32 + y * 32 + x + 1);
					}
				}
			}
		}

		return chunk;
	}
}
