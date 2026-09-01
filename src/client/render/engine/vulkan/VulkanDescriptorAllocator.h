#pragma once

#include "volk.h"

#include "client/render/engine/DescriptorAllocator.h"

namespace voxel_game::client::render::vulkan {
	class VulkanEngine;

	VkDescriptorType toVKDescriptorType(DescriptorType type);

	VkImageLayout toVKImageLayout(DescriptorType type);

	class VulkanDescriptorAllocator : public DescriptorAllocator {
	public:
		VulkanDescriptorAllocator(VulkanEngine* vulkanEngine, const std::vector<DescriptorBinding>& bindings, uint32_t maxSets, ShaderStage shaderStages);

		void clearDescriptors() override;

		std::unique_ptr<DescriptorSet> allocate() override;

		[[nodiscard]] DescriptorType getDescriptorType(uint32_t binding);

		~VulkanDescriptorAllocator() override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
		VkDescriptorPool mDescriptorPool = nullptr;
		VkDescriptorSetLayout mDescriptorSetLayout = nullptr;
		std::vector<DescriptorBinding> mBindings;
	};

	class VulkanDescriptorAllocatorBuilder : public DescriptorAllocatorBuilder {
	public:
		explicit VulkanDescriptorAllocatorBuilder(VulkanEngine* vulkanEngine);

		std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) override;

	private:
		VulkanEngine* mVulkanEngine = nullptr;
	};
}
