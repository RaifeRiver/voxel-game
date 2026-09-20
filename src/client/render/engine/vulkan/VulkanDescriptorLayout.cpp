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

#include "VulkanDescriptorLayout.h"

#include "VulkanDescriptorAllocator.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::engine::vulkan {
	VkDescriptorType toVKDescriptorType(const DescriptorType type) {
		switch (type) {
			case DescriptorType::SAMPLED_TEXTURE:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::TEXTURE:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case DescriptorType::IMAGE:
				return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case DescriptorType::SAMPLER:
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			case DescriptorType::UNIFORM_BUFFER:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::STORAGE_BUFFER:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			default:
				throw std::runtime_error("Unsupported descriptor type");
		}
	}

	VkImageLayout toVKImageLayout(const DescriptorType type) {
		switch (type) {
			case DescriptorType::SAMPLED_TEXTURE:
			case DescriptorType::TEXTURE:
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case DescriptorType::IMAGE:
				return VK_IMAGE_LAYOUT_GENERAL;
			default:
				throw std::runtime_error("Unsupported descriptor type");
		}
	}

	VulkanDescriptorLayout::VulkanDescriptorLayout(VulkanEngine* vulkanEngine, const std::vector<DescriptorType>& bindings) : DescriptorLayout(bindings), mVulkanEngine(vulkanEngine) {
		std::vector<VkDescriptorSetLayoutBinding> vulkanBindings;
		vulkanBindings.reserve(bindings.size());
		for (uint32_t binding = 0; binding < bindings.size(); binding++) {
			vulkanBindings.push_back(VkDescriptorSetLayoutBinding{
				.binding = binding,
				.descriptorType = toVKDescriptorType(bindings[binding]),
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_ALL
			});
		}
		const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = vulkanBindings.data()
		};
		vulkan_util::vkCheck(vkCreateDescriptorSetLayout(vulkanEngine->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout));
	}

	std::unique_ptr<DescriptorAllocator> VulkanDescriptorLayout::createAllocator(uint32_t maxSets) {
		return std::make_unique<VulkanDescriptorAllocator>(mVulkanEngine, this, maxSets);
	}

	VulkanDescriptorLayout::~VulkanDescriptorLayout() {
		vkDestroyDescriptorSetLayout(mVulkanEngine->getDevice(), mDescriptorSetLayout, nullptr);
	}

	VulkanDescriptorLayoutBuilder::VulkanDescriptorLayoutBuilder(VulkanEngine* vulkanEngine) : mVulkanEngine(vulkanEngine) {}

	std::unique_ptr<DescriptorLayout> VulkanDescriptorLayoutBuilder::build() {
		const std::vector<std::optional<DescriptorType>> optionalBindings = getBindings();
		std::vector<DescriptorType> bindings;
		for (uint32_t binding = 0; binding < optionalBindings.size(); binding++) {
			if (optionalBindings[binding]) {
				bindings.push_back(optionalBindings[binding].value());
			}
			else {
				throw std::runtime_error("Missing descriptor binding " + std::to_string(binding));
			}
		}
		return std::make_unique<VulkanDescriptorLayout>(mVulkanEngine, bindings);
	}
}
