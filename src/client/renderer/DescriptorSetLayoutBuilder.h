#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace voxel_game::client::renderer {
	class DescriptorSetLayoutBuilder {
	public:
		void addBinding(uint32_t binding, VkDescriptorType descriptorType, uint32_t count = 1);

		void clear();

		VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags stageFlags);

	private:
		std::vector<VkDescriptorSetLayoutBinding> mBindings = {};
	};
}
