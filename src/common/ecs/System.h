#pragma once

#include <functional>

namespace voxel_game::ecs {
	class ECSRegistry;

	enum class SystemStage {
		PRE_RENDER,
		BACKGROUND_RENDER,
		RENDER,
		POST_RENDER,
		COUNT
	};

	using SystemFunction = std::function<void(ECSRegistry&, float)>;

	class ISystem {
	public:
		virtual void runStage(SystemStage stage, ECSRegistry& registry, float deltaTime) = 0;

		virtual uint32_t getID() = 0;

		virtual ~ISystem() = default;

	protected:
		static inline uint32_t sNextID = 0;
	};

	// ReSharper disable once CppTemplateParameterNeverUsed
	template <typename T> class System : public ISystem {
	public:
		uint32_t getID() override {
			return getID_();
		}

		[[nodiscard]] static uint32_t getID_() {
			static uint32_t id = sNextID++;
			return id;
		}
	};
}
