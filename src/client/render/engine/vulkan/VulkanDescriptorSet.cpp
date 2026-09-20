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

#include "VulkanDescriptorSet.h"

#include "VulkanBuffer.h"
#include "VulkanDescriptorAllocator.h"
#include "VulkanDescriptorLayout.h"

namespace voxel_game::client::render::engine::vulkan {
	// ReSharper disable CppParameterMayBeConst
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanEngine* vulkanEngine, VulkanDescriptorAllocator* descriptorAllocator, VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet) : mVulkanEngine(vulkanEngine), mDescriptorAllocator(descriptorAllocator), mDescriptorPool(descriptorPool), mDescriptorSet(descriptorSet) {}
	// ReSharper restore CppParameterMayBeConst

	void VulkanDescriptorSet::setBinding(const uint32_t binding, GPUImage* image) {
		const auto* vulkanImage = dynamic_cast<VulkanImage*>(image);

		const DescriptorType descriptorType = mDescriptorAllocator->getDescriptorType(binding);
		VkDescriptorImageInfo descriptorImageInfo = {
			.imageView = vulkanImage->getImageView(),
			.imageLayout = toVKImageLayout(descriptorType)
		};
		const VkWriteDescriptorSet writeDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.descriptorCount = 1,
			.descriptorType = toVKDescriptorType(descriptorType),
			.pImageInfo = &descriptorImageInfo
		};
		vkUpdateDescriptorSets(mVulkanEngine->getDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}

	void VulkanDescriptorSet::setBinding(const uint32_t binding, GPUBuffer* buffer) {
		const auto* vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffer);

		const DescriptorType descriptorType = mDescriptorAllocator->getDescriptorType(binding);
		VkDescriptorBufferInfo descriptorBufferInfo = {
			.buffer = vulkanBuffer->getBuffer(),
			.range = VK_WHOLE_SIZE
		};
		const VkWriteDescriptorSet writeDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.descriptorCount = 1,
			.descriptorType = toVKDescriptorType(descriptorType),
			.pBufferInfo = &descriptorBufferInfo
		};
		vkUpdateDescriptorSets(mVulkanEngine->getDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
}
