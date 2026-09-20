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

#include "OpenGLDescriptorAllocator.h"

#include "OpenGLDescriptorSet.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::engine::opengl {
	OpenGLDescriptorAllocator::OpenGLDescriptorAllocator(const uint32_t maxSets, const std::vector<DescriptorType>& descriptorBindings) : mMaxSets(maxSets), mDescriptorBindings(descriptorBindings) {}

	void OpenGLDescriptorAllocator::clearDescriptors() {
		for (OpenGLDescriptorSet* descriptorSet: mDescriptorSets) {
			descriptorSet->destroy();
		}
		mDescriptorSets.clear();
	}

	std::unique_ptr<DescriptorSet> OpenGLDescriptorAllocator::allocate() {
		if (mDescriptorSets.size() == mMaxSets) {
			throw std::runtime_error("Too many descriptor sets allocated");
		}
		auto descriptorSet = std::make_unique<OpenGLDescriptorSet>(this);
		mDescriptorSets.push_back(descriptorSet.get());
		return descriptorSet;
	}
}
