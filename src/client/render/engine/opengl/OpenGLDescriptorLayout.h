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

#include "client/render/engine/DescriptorLayout.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLDescriptorLayout : public DescriptorLayout {
	public:
		explicit OpenGLDescriptorLayout(const std::vector<DescriptorType>& bindings);

		std::unique_ptr<DescriptorAllocator> createAllocator(uint32_t maxSets) override;
	};

	class OpenGLDescriptorLayoutBuilder : public DescriptorLayoutBuilder {
	public:
		std::unique_ptr<DescriptorLayout> build() override;
	};
}
