#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace voxel_game::ecs {
	class ECSRegistry;

	using SystemFunction = std::function<void(ECSRegistry&, float)>;

	class System {
	public:
		System();

		[[nodiscard]] uint32_t getID() const {
			return mID;
		}

		virtual void preRender(ECSRegistry& registry, float deltaTime) {}

		virtual void backgroundRender(ECSRegistry& registry, float deltaTime) {}

		virtual void render(ECSRegistry& registry, float deltaTime) {}

		virtual void postRender(ECSRegistry& registry, float deltaTime) {}

		virtual ~System() = default;

	private:
		static inline uint32_t sNextID = 0;

		uint32_t mID;
	};

	enum class Stage {
		PRE_RENDER,
		BACKGROUND_RENDER,
		RENDER,
		POST_RENDER,
		COUNT
	};

	class SystemManager {
	public:
		void registerSystem(Stage stage, const SystemFunction& system);

		template <std::derived_from<System> T> uint32_t registerSystem(const T& system) {
			mSystems.push_back(std::make_unique<System>(system));
			return mSystems.back()->getID();
		}

		template <std::derived_from<System> T, typename... Args> uint32_t createSystem(Args&&... args) {
			mSystems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
			return mSystems.back()->getID();
		}

		void runSystems(ECSRegistry& registry, float deltaTime);

		void removeSystem(uint32_t id);

	private:
		std::vector<SystemFunction> mStages[static_cast<size_t>(Stage::COUNT)];
		std::vector<std::unique_ptr<System>> mSystems;
	};
}
