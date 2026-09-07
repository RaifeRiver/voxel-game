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

#include <string>

#include "Pipeline.h"

namespace voxel_game::client::render::engine {
	class ComputePipeline : public Pipeline {
	public:
		ComputePipeline();

		void dispatch(const uint32_t x, const uint32_t y = 1, const uint32_t z = 1, const std::string& label = "Unknown compute pipeline") {
			dispatch_(x, y, z, label);
		}

	protected:
		virtual void dispatch_(uint32_t x, uint32_t y, uint32_t z, const std::string& label) = 0;
	};
}
