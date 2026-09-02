#include "VulkanShader.h"

namespace voxel_game::client::render::engine::vulkan {
	VkShaderStageFlags toVKShaderStage(const ShaderStage shaderStage) {
		VkShaderStageFlags shaderStageFlags = 0;
		if (shaderStage & ShaderStage::VERTEX) {
			shaderStageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
		}
		if (shaderStage & ShaderStage::FRAGMENT) {
			shaderStageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		if (shaderStage & ShaderStage::COMPUTE) {
			shaderStageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
		}
		return shaderStageFlags;
	}
}
