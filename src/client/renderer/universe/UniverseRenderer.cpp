#include "UniverseRenderer.h"

#include <cmath>

#include "client/VoxelGameClient.h"
#include "client/renderer/RenderContext.h"
#include "client/renderer/VulkanUtil.h"
#include "client/resource/ShaderResource.h"

namespace voxel_game::client::renderer::universe {
	void UniverseRenderer::init(RenderContext *renderContext) {
		initPipelines(renderContext);
	}

	void UniverseRenderer::render(RenderContext *renderContext, const VkCommandBuffer commandBuffer) const {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mGradientPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mGradientPipelineLayout, 0, 1, &renderContext->getDrawImageDescriptorSet(), 0, nullptr);

		GradientPushConstants pushConstants = {};
		auto frame = static_cast<float>(renderContext->getCurrentFrame());
		pushConstants.r = std::abs(std::sin(frame / 900.0f));
		pushConstants.g = std::abs(std::sin((frame + 300.0f) / 900.0f));
		pushConstants.b = std::abs(std::sin((frame + 600.0f) / 900.0f));
		vkCmdPushConstants(commandBuffer, mGradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(GradientPushConstants), &pushConstants);

		vkCmdDispatch(commandBuffer, std::ceil(renderContext->getSwapchainExtent().width / 16.0f), std::ceil(renderContext->getSwapchainExtent().height / 16.0f), 1);
	}

	void UniverseRenderer::destroy(const RenderContext *renderContext) const {
		vkDeviceWaitIdle(renderContext->getDevice());

		vkDestroyPipelineLayout(renderContext->getDevice(), mGradientPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mGradientPipeline, nullptr);
	}

	void UniverseRenderer::initPipelines(RenderContext *renderContext) {
		VkPipelineLayoutCreateInfo gradientPipelineLayoutCreateInfo = {};
		gradientPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		gradientPipelineLayoutCreateInfo.setLayoutCount = 1;
		gradientPipelineLayoutCreateInfo.pSetLayouts = &renderContext->getDrawImageDescriptorSetLayout();

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.size = sizeof(GradientPushConstants);
		gradientPipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		gradientPipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		VK_CHECK(vkCreatePipelineLayout(renderContext->getDevice(), &gradientPipelineLayoutCreateInfo, nullptr, &mGradientPipelineLayout));

		auto* gradientShader = VoxelGameClient::getClient()->getResourceManager()->load<resource::ShaderResource>("gradient", "shaders/gradient.comp.spv");
		VkPipelineShaderStageCreateInfo gradientShaderStageCreateInfo = {};
		gradientShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		gradientShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		gradientShaderStageCreateInfo.module = gradientShader->getShaderModule();
		gradientShaderStageCreateInfo.pName = "main";

		VkComputePipelineCreateInfo gradientPipelineCreateInfo = {};
		gradientPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		gradientPipelineCreateInfo.stage = gradientShaderStageCreateInfo;
		gradientPipelineCreateInfo.layout = mGradientPipelineLayout;
		VK_CHECK(vkCreateComputePipelines(renderContext->getDevice(), nullptr, 1, &gradientPipelineCreateInfo, nullptr, &mGradientPipeline));

		VoxelGameClient::getClient()->getResourceManager()->unload("gradient");
	}
}
