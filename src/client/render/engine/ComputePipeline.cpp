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

#include "ComputePipeline.h"

namespace voxel_game::client::render::engine {
	ComputePipeline::ComputePipeline() : Pipeline(PipelineType::COMPUTE) {}

	ComputePipelineBuilder::ComputePipelineBuilder(Shader computeShader) : mComputeShader(std::move(computeShader)) {}

	ComputePipelineBuilder * ComputePipelineBuilder::descriptorLayout(const uint32_t set, DescriptorLayout *descriptorLayout) {
		if (mDescriptorLayouts.size() <= set) {
			mDescriptorLayouts.resize(set + 1);
		}
		if (mDescriptorLayouts[set]) {
			throw std::runtime_error("Descriptor layout for set " + std::to_string(set) + " already exists");
		}
		mDescriptorLayouts[set] = descriptorLayout;
		return this;
	}
}
