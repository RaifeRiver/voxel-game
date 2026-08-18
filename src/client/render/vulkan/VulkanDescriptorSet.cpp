#include "VulkanDescriptorSet.h"

namespace voxel_game::client::render::vulkan {
	// ReSharper disable CppParameterMayBeConst
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanEngine* vulkanEngine, VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet) : mVulkanEngine(vulkanEngine), mDescriptorPool(descriptorPool), mDescriptorSet(descriptorSet) {}
	// ReSharper restore CppParameterMayBeConst
}
