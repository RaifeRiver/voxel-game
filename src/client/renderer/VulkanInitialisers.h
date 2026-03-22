#pragma once

#include <vulkan/vulkan.h>

namespace voxel_game::client::renderer::initialisers {
	VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags);

	VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool, uint32_t count);

	VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags);

	VkSemaphoreCreateInfo semaphoreCreateInfo(VkSemaphoreCreateFlags flags = 0);

	VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlags flags);

	VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlagBits aspectMask);

	VkSemaphoreSubmitInfo semaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags2 stageMask);

	VkCommandBufferSubmitInfo commandBufferSubmitInfo(VkCommandBuffer commandBuffer);

	VkSubmitInfo2 submitInfo(const VkCommandBufferSubmitInfo *commandBufferSubmitInfo, const VkSemaphoreSubmitInfo *signalSemaphoreSubmitInfo, const VkSemaphoreSubmitInfo *waitSemaphoreSubmitInfo);

	VkImageCreateInfo imageCreateInfo(VkFormat format, VkExtent3D extent, VkImageUsageFlags usage);

	VkImageViewCreateInfo imageViewCreateInfo(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
}