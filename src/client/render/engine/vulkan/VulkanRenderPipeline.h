#pragma once

#include "volk.h"

#include "client/render/engine/RenderPipeline.h"

namespace voxel_game::client::render::vulkan {
	class VulkanEngine;

	VkPrimitiveTopology toVKPrimitiveTopology(PrimitiveTopology topology);

	VkPolygonMode toVKPolygonMode(PolygonMode mode);

	VkCullModeFlags toVKCullMode(CullMode mode);

	VkFrontFace toVKFrontFace(FrontFace face);

	class VulkanRenderPipeline : public RenderPipeline {
	public:
		VulkanRenderPipeline(VulkanEngine* vulkanEngine, const RenderPipelineBuilder* builder);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		void setPushConstants(void* pushConstants) override;

		void bindIndexBuffer(GPUBuffer* buffer) override;

		~VulkanRenderPipeline() override;

	protected:
		void draw_(uint32_t vertexCount, uint32_t firstVertex) override;

		void drawIndexed_(uint32_t indexCount, uint32_t firstIndex) override;

	private:
		VulkanEngine* mVulkanEngine;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		VkPipeline mPipeline = nullptr;
		VkPipelineLayout mPipelineLayout = nullptr;
		uint32_t mPushConstantsSize = 0;
	};

	class VulkanRenderPipelineBuilder : public RenderPipelineBuilder {
	public:
		VulkanRenderPipelineBuilder(VulkanEngine* vulkanEngine, const std::string& vertexShader, const std::string& fragmentShader);

		std::unique_ptr<RenderPipeline> build() override;

	private:
		VulkanEngine* mVulkanEngine;
	};
}
