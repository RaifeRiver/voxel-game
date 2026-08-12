#include "SystemManager.h"

#include "ECSRegistry.h"

namespace voxel_game::ecs {
	void SystemManager::registerSystem(Stage stage, const System& system) {
		mStages[static_cast<size_t>(stage)].push_back(system);
	}

	void SystemManager::runSystems(ECSRegistry &registry, const float deltaTime) {
		for (std::vector<System>& stage: mStages) {
			for (System& system : stage) {
				system(registry, deltaTime);
			}
		}
	}
}