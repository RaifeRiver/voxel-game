#pragma once

#include <functional>

namespace voxel_game::ecs {
	class ECSRegistry;

	using System = std::function<void(ECSRegistry&, float)>;

	enum class Stage {
		PRE_RENDER,
		BACKGROUND_RENDER,
		POST_RENDER,
		COUNT
	};

	class SystemManager {
	public:
		void registerSystem(Stage stage, const System& system);

		void runSystems(ECSRegistry& registry, float deltaTime);

	private:
		std::vector<System> mStages[static_cast<size_t>(Stage::COUNT)];
	};
}