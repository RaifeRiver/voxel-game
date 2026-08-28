#pragma once

#include <cstdint>

#include "glm/vec3.hpp"

#include "common/ecs/System.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "common/util/GLMHasher.h"

namespace voxel_game::universe {
	constexpr double LOAD_DISTANCE_THRESHOLD = 64.0;

	class UniverseLoader : public ecs::System<UniverseLoader> {
	public:
		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unordered_map<glm::i64vec3, uint64_t> mLoadedSectors;

		void loadSector(const glm::i64vec3& sector);

		void unloadSector(const glm::i64vec3& sector);
	};
}
