#pragma once

#include "volk.h"

#include "client/render/RenderPipeline.h"

namespace voxel_game::client::render::vulkan {
	class VulkanEngine;

	VkPrimitiveTopology toVKPrimitiveTopology(PrimitiveTopology topology);

	VkPolygonMode toVKPolygonMode(PolygonMode mode);

	VkCullModeFlags toVKCullMode(CullMode mode);

	VkFrontFace toVKFrontFace(FrontFace face);

	class VulkanRenderPipeline : public RenderPipeline {
	public:
		VulkanRenderPipeline(VulkanEngine* vulkanEngine, const RenderPipelineBuilder* builder);

	private:
		VulkanEngine* mVulkanEngine;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		VkPipeline mPipeline = nullptr;
		VkPipelineLayout mPipelineLayout = nullptr;
	};

	class VulkanRenderPipelineBuilder : public RenderPipelineBuilder {
	public:
		VulkanRenderPipelineBuilder(VulkanEngine* vulkanEngine, const std::string& vertexShader, const std::string& fragmentShader);

		std::unique_ptr<RenderPipeline> build() override;

	private:
		VulkanEngine* mVulkanEngine;
	};
}
