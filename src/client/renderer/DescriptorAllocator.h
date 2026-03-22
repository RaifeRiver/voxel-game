#pragma once

#include <span>
#include <vulkan/vulkan.h>

namespace voxel_game::client::renderer {
	struct DescriptorPoolSizeRatio {
		VkDescriptorType type;
		float ratio;
	};

	class DescriptorAllocator {
	public:
		void init(VkDevice device, uint32_t maxSets, std::span<DescriptorPoolSizeRatio> poolSizes);

		void clear(VkDevice device) const;

		VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout) const;

		void destroy(VkDevice device) const;

	private:
		VkDescriptorPool mPool = nullptr;
	};
}
