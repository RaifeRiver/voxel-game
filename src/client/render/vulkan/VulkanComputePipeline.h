#pragma once

#include <string>

#include "VulkanEngine.h"

#include "client/render/ComputePipeline.h"

namespace voxel_game::client::render::vulkan {
	class VulkanComputePipeline : public ComputePipeline {
	public:
		VulkanComputePipeline(VulkanEngine* vulkanEngine, const std::string& computeShader);

		~VulkanComputePipeline() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		VkShaderModule mComputeShader = nullptr;
		VkPipeline mPipeline = nullptr;
		VkPipelineLayout mPipelineLayout = nullptr;
	};
}
