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

#include "DescriptorLayout.h"

namespace voxel_game::client::render::engine {
	DescriptorLayout::DescriptorLayout(const std::vector<DescriptorType>& bindings) : mBindings(bindings) {}

	DescriptorLayoutBuilder* DescriptorLayoutBuilder::addBinding(const uint32_t binding, DescriptorType type) {
		if (mBindings.size() <= binding) {
			mBindings.resize(binding + 1);
		}
		if (mBindings[binding]) {
			throw std::runtime_error("Binding " + std::to_string(binding) + " already has a type");
		}
		mBindings[binding] = type;
		return this;
	}
}
