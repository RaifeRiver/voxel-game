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
#include <string>
#include <vector>

namespace voxel_game::client::render::engine::opengl::opengl_util {
	enum class PushConstantDataType {
		FLOAT,
		DOUBLE,
		INT,
		UINT
	};

	struct PushConstantType {
		uint32_t rows;
		uint32_t columns;
		PushConstantDataType type;
	};

	struct PushConstant {
		std::string name;
		int location;
		uint32_t offset;
		PushConstantType type;
	};

	std::string convertShader(const std::vector<uint32_t>& shaderData);

	std::vector<PushConstant> getPushConstants(size_t shaderCount, const std::vector<uint32_t>* shaderData, unsigned int program);

	void setPushConstantData(const std::vector<PushConstant>& pushConstants, void* data);
}
