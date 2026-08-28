#include "SystemManager.h"

#include "ECSRegistry.h"

namespace voxel_game::ecs {
	void SystemManager::registerSystem(Stage stage, const SystemFunction& system) {
		mStages[static_cast<size_t>(stage)].push_back(system);
	}

	void SystemManager::runSystems(ECSRegistry& registry, const float deltaTime) {
		for (size_t i = 0; i < static_cast<size_t>(Stage::COUNT); i++) {
			for (SystemFunction& system : mStages[i]) {
				system(registry, deltaTime);
			}
			for (const std::unique_ptr<ISystem>& system: mSystems) {
				system->runStage(static_cast<Stage>(i), registry, deltaTime);
			}
		}
	}

	void SystemManager::removeSystem(const uint32_t id) {
		for (size_t i = 0; i < mSystems.size(); i++) {
			if (mSystems[i]->getID() == id) {
				mSystems.erase(mSystems.begin() + static_cast<uint32_t>(i));
				break;
			}
		}
	}
}
