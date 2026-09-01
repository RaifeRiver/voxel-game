#pragma once

#include <cstdint>
#include <stdexcept>

#include "simdjson/simdjson.h"

namespace voxel_game::ecs {
	struct IComponent {
		virtual void loadFromJSON(simdjson::dom::element json) {
			throw std::runtime_error("IComponent::loadFromJSON() not implemented");
		}

		virtual ~IComponent() = default;

	protected:
		static inline uint32_t sNextID = 0;
	};

	// ReSharper disable once CppTemplateParameterNeverUsed
	template <typename T> struct Component : IComponent {
		[[nodiscard]] static uint32_t getID() {
			static uint32_t id = sNextID++;
			return id;
		}
	};

	template <typename T> struct MarkerComponent : Component<T> {
		void loadFromJSON(const simdjson::dom::element json) override {
			if (json.type() != simdjson::dom::element_type::BOOL || !json.get_bool()) {
				throw std::runtime_error("Marker component expects only true in json files");
			}
		}
	};
}
