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

#include <memory>
#include <optional>

#include "DescriptorAllocator.h"

namespace voxel_game::client::render::engine {
	enum class DescriptorType {
		SAMPLED_TEXTURE,
		TEXTURE,
		IMAGE,
		SAMPLER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER
	};

	class DescriptorLayout {
	public:
		explicit DescriptorLayout(const std::vector<DescriptorType>& bindings);

		virtual std::unique_ptr<DescriptorAllocator> createAllocator(uint32_t maxSets) = 0;

		[[nodiscard]] const std::vector<DescriptorType>& getBindings() const {
			return mBindings;
		}

		virtual ~DescriptorLayout() = default;

	private:
		std::vector<DescriptorType> mBindings;
	};

	class DescriptorLayoutBuilder {
	public:
		DescriptorLayoutBuilder* addBinding(uint32_t binding, DescriptorType type);

		virtual std::unique_ptr<DescriptorLayout> build() = 0;

		[[nodiscard]] const std::vector<std::optional<DescriptorType>>& getBindings() const {
			return mBindings;
		}

		virtual ~DescriptorLayoutBuilder() = default;

	private:
		std::vector<std::optional<DescriptorType>> mBindings;
	};
}
