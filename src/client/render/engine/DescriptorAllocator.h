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
#include <memory>
#include <utility>
#include <vector>

#include "DescriptorSet.h"
#include "Shader.h"

namespace voxel_game::client::render::engine {
	enum class DescriptorType {
		SAMPLED_TEXTURE,
		TEXTURE,
		IMAGE,
		SAMPLER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER
	};

	class DescriptorAllocator {
	public:
		virtual void clearDescriptors() = 0;

		virtual std::unique_ptr<DescriptorSet> allocate() = 0;

		virtual ~DescriptorAllocator() = default;
	};

	using DescriptorBinding = std::pair<uint32_t, DescriptorType>;

	class DescriptorAllocatorBuilder {
	public:
		DescriptorAllocatorBuilder* addBinding(uint32_t binding, DescriptorType type);

		virtual std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) = 0;

		virtual ~DescriptorAllocatorBuilder() = default;

	protected:
		std::vector<DescriptorBinding> mBindings;
	};
}
