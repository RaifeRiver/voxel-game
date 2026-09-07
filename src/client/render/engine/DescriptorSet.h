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

#include "GPUBuffer.h"
#include "GPUImage.h"

namespace voxel_game::client::render::engine {
	class DescriptorSet {
	public:
		virtual void setBinding(uint32_t binding, GPUImage* image) = 0;

		virtual void setBinding(uint32_t binding, GPUBuffer* buffer) = 0;

		virtual ~DescriptorSet() = default;
	};
}
