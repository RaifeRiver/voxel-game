#pragma once

#include "volk.h"

#include "client/render/DescriptorAllocator.h"

namespace voxel_game::client::render::vulkan {
	class VulkanEngine;

	VkDescriptorType toVKDescriptorType(DescriptorType type);

	class VulkanDescriptorAllocator : public DescriptorAllocator {
	public:
		VulkanDescriptorAllocator(VulkanEngine* vulkanEngine, const std::vector<DescriptorBinding>& bindings, uint32_t maxSets, ShaderStage shaderStages);

		void clearDescriptors() override;

		std::unique_ptr<DescriptorSet> allocate() override;

		~VulkanDescriptorAllocator() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		VkDescriptorPool mDescriptorPool = nullptr;
		VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
	};

	class VulkanDescriptorAllocatorBuilder : public DescriptorAllocatorBuilder {
	public:
		explicit VulkanDescriptorAllocatorBuilder(VulkanEngine* vulkanEngine);

		std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
	};
}
