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

#include "client/render/engine/DescriptorAllocator.h"

namespace voxel_game::client::render::engine::vulkan {
	class VulkanEngine;

	VkDescriptorType toVKDescriptorType(DescriptorType type);

	VkImageLayout toVKImageLayout(DescriptorType type);

	class VulkanDescriptorAllocator : public DescriptorAllocator {
	public:
		VulkanDescriptorAllocator(VulkanEngine* vulkanEngine, const std::vector<DescriptorBinding>& bindings, uint32_t maxSets, ShaderStage shaderStages);

		void clearDescriptors() override;

		std::unique_ptr<DescriptorSet> allocate() override;

		[[nodiscard]] DescriptorType getDescriptorType(uint32_t binding);

		~VulkanDescriptorAllocator() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		VkDescriptorPool mDescriptorPool = nullptr;
		VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
		std::vector<DescriptorBinding> mBindings;
	};

	class VulkanDescriptorAllocatorBuilder : public DescriptorAllocatorBuilder {
	public:
		explicit VulkanDescriptorAllocatorBuilder(VulkanEngine* vulkanEngine);

		std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
	};
}
