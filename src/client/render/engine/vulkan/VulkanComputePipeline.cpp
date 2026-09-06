#include "VulkanComputePipeline.h"

#include <vector>

#include "spirv_cross.hpp"
#include "tracy/TracyVulkan.hpp"
#include "volk.h"

#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::engine::vulkan {
	VulkanComputePipeline::VulkanComputePipeline(VulkanEngine* vulkanEngine, const Shader& computeShader) : mVulkanEngine(vulkanEngine) {
		const std::vector<uint32_t>& computeShaderData = computeShader.getSPIRV();

		const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = computeShaderData.size() * sizeof(uint32_t),
			.pCode = computeShaderData.data()
		};
		VkShaderModule computeShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &shaderModuleCreateInfo, nullptr, &computeShaderModule));

		mDescriptorSetLayouts = vulkan_util::createDescriptorSetLayouts(mVulkanEngine, 1, &computeShaderData);

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

		for (const VkDescriptorSetLayout& descriptorSetLayout: mDescriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(mVulkanEngine->getDevice(), descriptorSetLayout, nullptr);
		}
	}
}
