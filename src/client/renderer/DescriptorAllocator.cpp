#include "DescriptorAllocator.h"

#include <vector>

namespace voxel_game::client::renderer {
	void DescriptorAllocator::init(VkDevice device, const uint32_t maxSets, const std::span<DescriptorPoolSizeRatio> poolSizes) {
		std::vector<VkDescriptorPoolSize> poolSizeVector;
		for (DescriptorPoolSizeRatio ratio: poolSizes) {
			poolSizeVector.push_back({ratio.type, static_cast<uint32_t>(ratio.ratio * maxSets)});
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizeVector.size());
		poolInfo.pPoolSizes = poolSizeVector.data();
		poolInfo.maxSets = maxSets;
		vkCreateDescriptorPool(device, &poolInfo, nullptr, &mPool);
	}

	void DescriptorAllocator::clear(const VkDevice device) const {
		vkResetDescriptorPool(device, mPool, 0);
	}

	VkDescriptorSet DescriptorAllocator::allocate(const VkDevice device, const VkDescriptorSetLayout layout) const {
		VkDescriptorSetAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocateInfo.descriptorPool = mPool;
		allocateInfo.descriptorSetCount = 1;
		allocateInfo.pSetLayouts = &layout;
		VkDescriptorSet descriptorSet;
		vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);
		return descriptorSet;
	}

	void DescriptorAllocator::destroy(const VkDevice device) const {
		vkDestroyDescriptorPool(device, mPool, nullptr);
	}
}
