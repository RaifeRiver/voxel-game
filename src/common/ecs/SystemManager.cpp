#include "SystemManager.h"

#include "ECSRegistry.h"

namespace voxel_game::ecs {
	void SystemManager::registerSystem(Stage stage, const System &system) {
		mStages[static_cast<size_t>(stage)].push_back(system);
	}

	void SystemManager::runStage(Stage stage, ECSRegistry &registry, const float deltaTime) {
		registry.executeCommands();
		for (System& system : mStages[static_cast<size_t>(stage)]) {
			system(registry, deltaTime);
		}
	}
}