#pragma once

#include <functional>

namespace voxel_game::ecs {
	class ECSRegistry;

	using System = std::function<void(ECSRegistry&, float)>;

	enum class Stage {
		PRE_UPDATE,
		BLOCK_UPDATE,
		ENTITY_UPDATE,
		PHYSICS,
		POST_UPDATE,
		WORLD_RENDER,
		UI_RENDER,
		COUNT
	};

	class SystemManager {
	public:
		void registerSystem(Stage stage, const System &system);

		void runStage(Stage stage, ECSRegistry& registry, float deltaTime);

	private:
		std::vector<System> mStages[static_cast<size_t>(Stage::COUNT)];
	};
}