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

#include "glm/vec3.hpp"

#include "common/ecs/System.h"

namespace voxel_game::universe {
	class UniverseLoader : public ecs::System<UniverseLoader> {
	public:
		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

		void loadSector(ecs::ECSRegistry& registry, const glm::i64vec3& sector);

		void unloadSector(ecs::ECSRegistry& registry, const glm::i64vec3& sector);

	private:
		uint32_t mLoadedSectors = 0;
	};
}
