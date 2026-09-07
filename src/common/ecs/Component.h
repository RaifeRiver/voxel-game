/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
