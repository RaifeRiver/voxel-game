#pragma once

#include <cstdint>

namespace voxel_game::ecs {
	class Entity {
	public:
		// ReSharper disable once CppNonExplicitConversionOperator
		operator uint32_t() const {
			return mID;
		}

	private:
		uint32_t mID;

		explicit Entity(const uint32_t id) : mID(id) {}

		friend class ECSRegistry;
		template <typename T> requires std::derived_from<T, Component<T>> friend class ComponentStorage;
	};
}