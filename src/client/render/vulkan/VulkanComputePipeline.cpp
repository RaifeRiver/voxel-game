#include "VulkanComputePipeline.h"

#include <vector>

#include "volk.h"

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
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &shaderModuleCreateInfo, nullptr, &mComputeShader));
	}

	VulkanComputePipeline::~VulkanComputePipeline() {
		vkDestroyShaderModule(mVulkanEngine->getDevice(), mComputeShader, nullptr);
	}
}
