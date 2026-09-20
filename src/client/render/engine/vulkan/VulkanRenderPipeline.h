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

#include "client/render/engine/RenderPipeline.h"

namespace voxel_game::client::render::engine::vulkan {
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
		void draw_(uint32_t vertexCount, uint32_t firstVertex, const std::string& label) override;

		void drawIndexed_(uint32_t indexCount, uint32_t firstIndex, const std::string& label) override;

		void drawIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) override;

		void drawIndexedIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) override;

	private:
		VulkanEngine* mVulkanEngine;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		VkPipeline mPipeline = nullptr;
		VkPipelineLayout mPipelineLayout = nullptr;
		uint32_t mPushConstantsSize = 0;
		VkShaderStageFlags mPushConstantStages = 0;
	};

	class VulkanRenderPipelineBuilder : public RenderPipelineBuilder {
	public:
		VulkanRenderPipelineBuilder(VulkanEngine* vulkanEngine, const Shader& vertexShader, const Shader& fragmentShader);

		std::unique_ptr<RenderPipeline> build() override;

	private:
		VulkanEngine* mVulkanEngine;
	};
}
