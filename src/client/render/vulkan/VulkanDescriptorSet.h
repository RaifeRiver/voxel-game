#pragma once

#include "VulkanEngine.h"
#include "client/render/DescriptorSet.h"

namespace voxel_game::client::render::vulkan {
    class VulkanDescriptorSet : public DescriptorSet {
    public:
    	VulkanDescriptorSet(VulkanEngine* vulkanEngine, VkDescriptorPool descriptorPool, VkDescriptorSet descriptorSet);

    private:
    	VulkanEngine* mVulkanEngine = nullptr;
    	VkDescriptorPool mDescriptorPool = nullptr;
        VkDescriptorSet mDescriptorSet = nullptr;
    };
}