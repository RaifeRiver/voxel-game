#pragma once

#include "VulkanEngine.h"
#include "client/render/DescriptorSet.h"

namespace voxel_game::client::render::vulkan {
	class VulkanDescriptorAllocator;

	class VulkanDescriptorSet : public DescriptorSet {
    public:
    	VulkanDescriptorSet(VulkanEngine* vulkanEngine, VulkanDescriptorAllocator* descriptorAllocator, VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet);

	    void setBinding(uint32_t binding, GPUImage* image) override;

	    [[nodiscard]] VkDescriptorSet getDescriptorSet() const {
		    return mDescriptorSet;
	    }

    private:
    	VulkanEngine* mVulkanEngine = nullptr;
		VulkanDescriptorAllocator* mDescriptorAllocator = nullptr;
    	VkDescriptorPool mDescriptorPool = nullptr;
        VkDescriptorSet mDescriptorSet = nullptr;
    };
}