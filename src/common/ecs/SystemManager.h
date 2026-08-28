#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "System.h"

namespace voxel_game::ecs {
	class ECSRegistry;

	class SystemManager {
	public:
		void registerSystem(Stage stage, const SystemFunction& system);

		template <typename T> requires std::derived_from<T, System<T>> void registerSystem(const T& system) {
			const uint32_t id = system.getID();
			if (id >= mSystems.size()) {
				mSystems.resize(id + 1);
			}
			mSystems[id] = std::unique_ptr<T>(*system);
		}

		template <typename T, typename... Args> requires std::derived_from<T, System<T>> void createSystem(Args&&... args) {
			const uint32_t id = T::getID_();
			if (id >= mSystems.size()) {
				mSystems.resize(id + 1);
			}
			mSystems[id] = std::make_unique<T>(std::forward<Args>(args)...);
		}

		void runSystems(ECSRegistry& registry, float deltaTime);

		void removeSystem(uint32_t id);

		template <typename T> requires std::derived_from<T, System<T>> void removeSystem() {
			const uint32_t id = T::getID_();
			mSystems[id] = nullptr;
		}

	private:
		std::vector<SystemFunction> mStages[static_cast<size_t>(Stage::COUNT)];
		std::vector<std::unique_ptr<ISystem>> mSystems;
	};
}
