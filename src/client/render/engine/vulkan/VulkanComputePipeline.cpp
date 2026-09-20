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

#include "VulkanComputePipeline.h"

#include <vector>

#include "spirv_cross.hpp"
#include "tracy/TracyVulkan.hpp"
#include "volk.h"
#include "VulkanDescriptorLayout.h"

#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::engine::vulkan {
	VulkanComputePipeline::VulkanComputePipeline(VulkanEngine* vulkanEngine, const ComputePipelineBuilder* builder) : mVulkanEngine(vulkanEngine) {
		const std::vector<uint32_t>& computeShaderData = builder->getComputeShader().getSPIRV();

		const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = computeShaderData.size() * sizeof(uint32_t),
			.pCode = computeShaderData.data()
		};
		VkShaderModule computeShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &shaderModuleCreateInfo, nullptr, &computeShaderModule));

		for (DescriptorLayout* layout : builder->getDescriptorLayouts()) {
			mDescriptorSetLayouts.push_back(reinterpret_cast<VulkanDescriptorLayout*>(layout)->getDescriptorSetLayout());
		}

		std::vector<VkPushConstantRange> pushConstantRanges = vulkan_util::getPushConstantRanges(1, &computeShaderData);
		for (const auto&[stageFlags, offset, size] : pushConstantRanges) {
			mPushConstantsSize = std::max(mPushConstantsSize, offset + size);
			mPushConstantStages |= stageFlags;
		}
		const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size()),
			.pSetLayouts = mDescriptorSetLayouts.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
			.pPushConstantRanges = pushConstantRanges.data()
		};
		vulkan_util::vkCheck(vkCreatePipelineLayout(mVulkanEngine->getDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout));

		const VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_COMPUTE_BIT,
			.module = computeShaderModule,
			.pName = "main",
		};
		const VkComputePipelineCreateInfo computePipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
			.stage = pipelineShaderStageCreateInfo,
			.layout = mPipelineLayout
		};
		vulkan_util::vkCheck(vkCreateComputePipelines(mVulkanEngine->getDevice(), nullptr, 1, &computePipelineCreateInfo, nullptr, &mPipeline));

		vkDestroyShaderModule(mVulkanEngine->getDevice(), computeShaderModule, nullptr);
	}

	void VulkanComputePipeline::bind() {
		vkCmdBindPipeline(mVulkanEngine->getCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipeline);
	}

	void VulkanComputePipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		// ReSharper disable once CppLocalVariableMayBeConst
		VkDescriptorSet vulkanDescriptorSet = dynamic_cast<VulkanDescriptorSet*>(descriptorSet)->getDescriptorSet();
		vkCmdBindDescriptorSets(mVulkanEngine->getCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, mPipelineLayout, set, 1, &vulkanDescriptorSet, 0, nullptr);
	}

	void VulkanComputePipeline::setPushConstants(void* pushConstants) {
		vkCmdPushConstants(mVulkanEngine->getCommandBuffer(), mPipelineLayout, mPushConstantStages, 0, mPushConstantsSize, pushConstants);
	}

	void VulkanComputePipeline::dispatch_(const uint32_t x, const uint32_t y, const uint32_t z, const std::string& label) {
		const VkCommandBuffer commandBuffer = mVulkanEngine->getCommandBuffer();
		TracyVkZoneTransient(mVulkanEngine->getTracyContext(), tracyZone, commandBuffer, label.c_str(), true);
		vkCmdDispatch(commandBuffer, x, y, z);
	}

	VulkanComputePipeline::~VulkanComputePipeline() {
		vkDestroyPipelineLayout(mVulkanEngine->getDevice(), mPipelineLayout, nullptr);
		vkDestroyPipeline(mVulkanEngine->getDevice(), mPipeline, nullptr);
	}

	VulkanComputePipelineBuilder::VulkanComputePipelineBuilder(VulkanEngine* vulkanEngine, const Shader& computeShader) : ComputePipelineBuilder(computeShader), mVulkanEngine(vulkanEngine) {}

	std::unique_ptr<ComputePipeline> VulkanComputePipelineBuilder::build() {
		return std::make_unique<VulkanComputePipeline>(mVulkanEngine, this);
	}
}
