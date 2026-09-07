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

#include "OpenGLDescriptorSet.h"
#include "client/render/engine/DescriptorAllocator.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLDescriptorAllocator : public DescriptorAllocator {
	public:
		explicit OpenGLDescriptorAllocator(uint32_t maxSets, const std::vector<DescriptorBinding>& descriptorBindings);

		void clearDescriptors() override;

		std::unique_ptr<DescriptorSet> allocate() override;

		[[nodiscard]] const std::vector<DescriptorBinding>& getDescriptorBindings() const {
			return mDescriptorBindings;
		}

	private:
		uint32_t mMaxSets;
		std::vector<OpenGLDescriptorSet*> mDescriptorSets = {};
		std::vector<DescriptorBinding> mDescriptorBindings;
	};

	class OpenGLDescriptorAllocatorBuilder : public DescriptorAllocatorBuilder {
	public:
		std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) override;
	};
}
