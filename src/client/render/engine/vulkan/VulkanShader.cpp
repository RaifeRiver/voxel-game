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

#include "VulkanShader.h"

namespace voxel_game::client::render::engine::vulkan {
	VkShaderStageFlags toVKShaderStage(const ShaderStage shaderStage) {
		VkShaderStageFlags shaderStageFlags = 0;
		if (shaderStage & ShaderStage::VERTEX) {
			shaderStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (shaderStage & ShaderStage::FRAGMENT) {
			shaderStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (shaderStage & ShaderStage::COMPUTE) {
			shaderStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return shaderStageFlags;
	}
}
