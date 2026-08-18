#include "VulkanComputePipeline.h"

#include <vector>

#include "spirv_cross.hpp"
#include "volk.h"

#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render::vulkan {
	VulkanComputePipeline::VulkanComputePipeline(VulkanEngine* vulkanEngine, const std::string& computeShader) : mVulkanEngine(vulkanEngine) {
		std::vector<uint32_t> computeShaderData = util::readFile<uint32_t>(computeShader);

		const VkShaderModuleCreateInfo shaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = computeShaderData.size() * sizeof(uint32_t),
			.pCode = computeShaderData.data()
		};
		VkShaderModule computeShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &shaderModuleCreateInfo, nullptr, &computeShaderModule));

		const spirv_cross::Compiler compiler(computeShaderData);
		const spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();

		std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
		auto processResources = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources, const VkDescriptorType descriptorType) {
			for (const spirv_cross::Resource& sampledImage: resources) {
				const uint32_t set = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);
				if (descriptorSetLayoutBindings.size() <= set) {
					descriptorSetLayoutBindings.resize(set + 1);
				}
				descriptorSetLayoutBindings[set].push_back({
					.binding = compiler.get_decoration(sampledImage.id, spv::DecorationBinding),
					.descriptorType = descriptorType,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
				});
			}
		};
		processResources(shaderResources.sampled_images, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		processResources(shaderResources.separate_images, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
		processResources(shaderResources.storage_images, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		processResources(shaderResources.separate_samplers, VK_DESCRIPTOR_TYPE_SAMPLER);
		processResources(shaderResources.uniform_buffers, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		processResources(shaderResources.storage_buffers, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

		mDescriptorSetLayouts.resize(descriptorSetLayoutBindings.size());
		for (size_t i = 0; i < descriptorSetLayoutBindings.size(); i++) {
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings[i].size()),
				.pBindings = descriptorSetLayoutBindings[i].data()
			};
			vulkan_util::vkCheck(vkCreateDescriptorSetLayout(mVulkanEngine->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayouts[i]));
		}

		const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size()),
			.pSetLayouts = mDescriptorSetLayouts.data()
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

	void VulkanComputePipeline::dispatch_(const uint32_t x, const uint32_t y, const uint32_t z) {
		vkCmdDispatch(mVulkanEngine->getCommandBuffer(), x, y, z);
	}

	VulkanComputePipeline::~VulkanComputePipeline() {
		vkDestroyPipelineLayout(mVulkanEngine->getDevice(), mPipelineLayout, nullptr);
		vkDestroyPipeline(mVulkanEngine->getDevice(), mPipeline, nullptr);

		for (const VkDescriptorSetLayout& descriptorSetLayout: mDescriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(mVulkanEngine->getDevice(), descriptorSetLayout, nullptr);
		}
	}
}
