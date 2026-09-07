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
