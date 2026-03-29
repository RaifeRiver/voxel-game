#include "DescriptorSetLayoutBuilder.h"

namespace voxel_game::client::renderer {
	void DescriptorSetLayoutBuilder::addBinding(const uint32_t binding, const VkDescriptorType descriptorType, uint32_t count) {
		VkDescriptorSetLayoutBinding bindingInfo = {};
		bindingInfo.binding = binding;
		bindingInfo.descriptorType = descriptorType;
		bindingInfo.descriptorCount = count;
		mBindings.push_back(bindingInfo);
	}

	void DescriptorSetLayoutBuilder::clear() {
		mBindings.clear();
	}

	VkDescriptorSetLayout DescriptorSetLayoutBuilder::build(const VkDevice device, const VkShaderStageFlags stageFlags) {
		for (VkDescriptorSetLayoutBinding &binding: mBindings) {
			binding.stageFlags = stageFlags;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(mBindings.size());
		layoutInfo.pBindings = mBindings.data();

		VkDescriptorSetLayout layout;
		vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);
		return layout;
	}
}
