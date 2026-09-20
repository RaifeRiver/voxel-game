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

#include "VulkanDescriptorAllocator.h"

#include <unordered_map>

#include "VulkanDescriptorLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::engine::vulkan {
	VulkanDescriptorAllocator::VulkanDescriptorAllocator(VulkanEngine* vulkanEngine, DescriptorLayout* descriptorLayout, const uint32_t maxSets) : mVulkanEngine(vulkanEngine), mBindings(descriptorLayout->getBindings()) {
		std::unordered_map<DescriptorType, uint32_t> typeCounts;
		for (const DescriptorType& type: mBindings) {
			auto it = typeCounts.find(type);
			if (it != typeCounts.end()) {
				it->second++;
			}
			else {
				typeCounts[type] = 1;
			}
		}
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		descriptorPoolSizes.reserve(typeCounts.size());
		for (auto& [type, count] : typeCounts) {
			descriptorPoolSizes.push_back({
				.type = toVKDescriptorType(type),
				.descriptorCount = count * maxSets
			});
		}

		const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = maxSets,
			.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size()),
			.pPoolSizes = descriptorPoolSizes.data()
		};
		vulkan_util::vkCheck(vkCreateDescriptorPool(mVulkanEngine->getDevice(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool));

		mDescriptorSetLayout = reinterpret_cast<VulkanDescriptorLayout*>(descriptorLayout)->getDescriptorSetLayout();
	}

	void VulkanDescriptorAllocator::clearDescriptors() {
		vkResetDescriptorPool(mVulkanEngine->getDevice(), mDescriptorPool, 0);
	}

	std::unique_ptr<DescriptorSet> VulkanDescriptorAllocator::allocate() {
		const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = mDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &mDescriptorSetLayout
		};
		VkDescriptorSet descriptorSet;
		vulkan_util::vkCheck(vkAllocateDescriptorSets(mVulkanEngine->getDevice(), &descriptorSetAllocateInfo, &descriptorSet));
		return std::make_unique<VulkanDescriptorSet>(mVulkanEngine, this, mDescriptorPool, descriptorSet);
	}

	DescriptorType VulkanDescriptorAllocator::getDescriptorType(const uint32_t binding) const {
		if (binding >= mBindings.size()) {
			throw std::runtime_error("No binding at index " + std::to_string(binding));
		}
		return mBindings[binding];
	}

	VulkanDescriptorAllocator::~VulkanDescriptorAllocator() {
		vkDestroyDescriptorPool(mVulkanEngine->getDevice(), mDescriptorPool, nullptr);
	}
}
