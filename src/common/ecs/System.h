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
#include <functional>

namespace voxel_game::ecs {
	class ECSRegistry;

	enum class SystemStage {
		UPDATE,
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
