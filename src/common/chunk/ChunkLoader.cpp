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
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadChunkEvent.h"
#include "common/event/LoadSectorEvent.h"
#include "common/event/UnloadChunkEvent.h"
#include "common/event/UnloadSectorEvent.h"
#include "common/universe/UniversePos.h"

namespace voxel_game::chunk {
	void ChunkLoader::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::UPDATE) {
			return;
		}

		ZoneScopedN("Loading chunks");

		const std::vector<event::LoadSectorEvent*> loadSectorEvents = registry.getEvents<event::LoadSectorEvent>();
		const std::vector<event::UnloadSectorEvent*> unloadSectorEvents = registry.getEvents<event::UnloadSectorEvent>();

		const std::vector<ecs::Entity>& chunkDataEntities = registry.getEntitiesWithComponents<ChunkData>();
		for (const ecs::Entity entity : chunkDataEntities) {
			ChunkData& chunkData = registry.getComponent<ChunkData>(entity);

			for (const event::UnloadSectorEvent* event : unloadSectorEvents) {
				const glm::i64vec3 sector = event->sector;
				for (int32_t x = 0; x < universe::SECTOR_SIZE / CHUNK_SIZE; x++) {
					for (int32_t y = 0; y < universe::SECTOR_SIZE / CHUNK_SIZE; y++) {
						for (int32_t z = 0; z < universe::SECTOR_SIZE / CHUNK_SIZE; z++) {
							glm::ivec3 chunk = glm::ivec3{sector} * glm::ivec3{universe::SECTOR_SIZE / CHUNK_SIZE} + glm::ivec3{x, y, z};
							if (chunkData.isLoaded(chunk)) {
								registry.pushEvent<event::UnloadChunkEvent>({entity, chunk});
								chunkData.chunks.erase(chunk);
							}
						}
					}
				}
			}

			for (const event::LoadSectorEvent* event : loadSectorEvents) {
				const glm::i64vec3 sector = event->sector;
				for (int32_t x = 0; x < universe::SECTOR_SIZE / CHUNK_SIZE; x++) {
					for (int32_t y = 0; y < universe::SECTOR_SIZE / CHUNK_SIZE; y++) {
						for (int32_t z = 0; z < universe::SECTOR_SIZE / CHUNK_SIZE; z++) {
							glm::ivec3 chunk = glm::ivec3{sector} * glm::ivec3{universe::SECTOR_SIZE / CHUNK_SIZE} + glm::ivec3{x, y, z};
							if (!chunkData.isLoaded(chunk)) {
								chunkData.chunks.emplace(chunk, createChunk(chunk, entity));
								registry.pushEvent<event::LoadChunkEvent>({entity, chunkData.chunks.at(chunk)});
							}
						}
					}
				}
			}
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
