#pragma once

#include <string>

#include "VulkanEngine.h"

#include "client/render/ComputePipeline.h"

namespace voxel_game::client::render::vulkan {
	class VulkanComputePipeline : public ComputePipeline {
	public:
		VulkanComputePipeline(VulkanEngine* vulkanEngine, const std::string& computeShader);

		void bind() override;

		void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) override;

		void setPushConstants(void* pushConstants) override;

		~VulkanComputePipeline() override;

	protected:
		void dispatch_(uint32_t x, uint32_t y, uint32_t z) override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		std::vector<VkDescriptorSetLayout> mDescriptorSetLayouts;
		VkPipeline mPipeline = nullptr;
		VkPipelineLayout mPipelineLayout = nullptr;
		uint32_t mPushConstantsSize = 0;
	};
}
