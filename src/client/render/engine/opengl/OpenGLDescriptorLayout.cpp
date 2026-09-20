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

#include "OpenGLDescriptorLayout.h"

#include "OpenGLDescriptorAllocator.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLDescriptorLayout::OpenGLDescriptorLayout(const std::vector<DescriptorType> &bindings) : DescriptorLayout(bindings) {}

	std::unique_ptr<DescriptorAllocator> OpenGLDescriptorLayout::createAllocator(uint32_t maxSets) {
		return std::make_unique<OpenGLDescriptorAllocator>(maxSets, getBindings());
	}

	std::unique_ptr<DescriptorLayout> OpenGLDescriptorLayoutBuilder::build() {
		const std::vector<std::optional<DescriptorType>> optionalBindings = getBindings();
		std::vector<DescriptorType> bindings;
		for (uint32_t binding = 0; binding < optionalBindings.size(); binding++) {
			if (optionalBindings[binding]) {
				bindings.push_back(optionalBindings[binding].value());
			}
			else {
				throw std::runtime_error("Missing descriptor binding " + std::to_string(binding));
			}
		}
		return std::make_unique<OpenGLDescriptorLayout>(bindings);
	}
}
