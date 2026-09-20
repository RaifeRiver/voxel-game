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

#include "volk.h"

#include "client/render/engine/DescriptorLayout.h"

namespace voxel_game::client::render::engine::vulkan {
	class VulkanEngine;

	VkDescriptorType toVKDescriptorType(DescriptorType type);

	VkImageLayout toVKImageLayout(DescriptorType type);

	class VulkanDescriptorLayout : public DescriptorLayout {
	public:
		explicit VulkanDescriptorLayout(VulkanEngine* vulkanEngine, const std::vector<DescriptorType>& bindings);

		std::unique_ptr<DescriptorAllocator> createAllocator(uint32_t maxSets) override;

		[[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const {
			return mDescriptorSetLayout;
		}

		~VulkanDescriptorLayout() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
	};

	class VulkanDescriptorLayoutBuilder : public DescriptorLayoutBuilder {
	public:
		explicit VulkanDescriptorLayoutBuilder(VulkanEngine* vulkanEngine);

		std::unique_ptr<DescriptorLayout> build() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
	};
}
