#pragma once

#include <cstdint>

namespace voxel_game::ecs {
	struct IComponent {
	protected:
		static inline uint32_t sNextID = 0;
	};

	// ReSharper disable once CppTemplateParameterNeverUsed
	template <typename T> struct Component : public IComponent {
		[[nodiscard]] static uint32_t getID() {
			static uint32_t id = sNextID++;
			return id;
		}
	};
}