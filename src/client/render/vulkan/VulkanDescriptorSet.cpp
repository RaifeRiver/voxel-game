#include "VulkanDescriptorSet.h"

#include "VulkanDescriptorAllocator.h"

namespace voxel_game::client::render::vulkan {
	// ReSharper disable CppParameterMayBeConst
	VulkanDescriptorSet::VulkanDescriptorSet(VulkanEngine* vulkanEngine, VulkanDescriptorAllocator* descriptorAllocator, VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet) : mVulkanEngine(vulkanEngine), mDescriptorAllocator(descriptorAllocator), mDescriptorPool(descriptorPool), mDescriptorSet(descriptorSet) {}
	// ReSharper restore CppParameterMayBeConst

	void VulkanDescriptorSet::setBinding(const uint32_t binding, GPUImage* image) {
		const auto* vulkanImage = dynamic_cast<VulkanImage*>(image);

		const DescriptorType descriptorType = mDescriptorAllocator->getDescriptorType(binding);
		VkDescriptorImageInfo descriptorImageInfo = {
			.imageView = vulkanImage->getImageView(),
			.imageLayout = toVKImageLayout(descriptorType)
		};
		const VkWriteDescriptorSet writeDescriptorSet = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = mDescriptorSet,
			.dstBinding = binding,
			.descriptorCount = 1,
			.descriptorType = toVKDescriptorType(descriptorType),
			.pImageInfo = &descriptorImageInfo
		};
		vkUpdateDescriptorSets(mVulkanEngine->getDevice(), 1, &writeDescriptorSet, 0, nullptr);
	}
}
