#include "VulkanInitialisers.h"

namespace voxel_game::client::renderer::initialisers {
	VkCommandPoolCreateInfo commandPoolCreateInfo(const uint32_t queueFamilyIndex, const VkCommandPoolCreateFlags flags) {
		VkCommandPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.queueFamilyIndex = queueFamilyIndex;
		info.flags = flags;
		return info;
	}

	VkCommandBufferAllocateInfo commandBufferAllocateInfo(const VkCommandPool commandPool, const uint32_t count) {
		VkCommandBufferAllocateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		info.commandPool = commandPool;
		info.commandBufferCount = count;
		info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		return info;
	}

	VkFenceCreateInfo fenceCreateInfo(const VkFenceCreateFlags flags) {
		VkFenceCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		info.flags = flags;
		return info;
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo(const VkSemaphoreCreateFlags flags) {
		VkSemaphoreCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		info.flags = flags;
		return info;
	}

	VkCommandBufferBeginInfo commandBufferBeginInfo(const VkCommandBufferUsageFlags flags) {
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags = flags;
		return info;
	}

	VkImageSubresourceRange imageSubresourceRange(const VkImageAspectFlagBits aspectMask) {
		VkImageSubresourceRange subImage = {};
		subImage.aspectMask = aspectMask;
		subImage.baseMipLevel = 0;
		subImage.levelCount = VK_REMAINING_MIP_LEVELS;
		subImage.baseArrayLayer = 0;
		subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;
		return subImage;
	}

	VkSemaphoreSubmitInfo semaphoreSubmitInfo(const VkSemaphore semaphore, const VkPipelineStageFlags2 stageMask) {
		VkSemaphoreSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		submitInfo.semaphore = semaphore;
		submitInfo.stageMask = stageMask;
		submitInfo.deviceIndex = 0;
		submitInfo.value = 1;
		return submitInfo;
	}

	VkCommandBufferSubmitInfo commandBufferSubmitInfo(const VkCommandBuffer commandBuffer) {
		VkCommandBufferSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
		info.commandBuffer = commandBuffer;
		info.deviceMask = 0;
		return info;
	}

	VkSubmitInfo2 submitInfo(const VkCommandBufferSubmitInfo *commandBufferSubmitInfo, const VkSemaphoreSubmitInfo *signalSemaphoreSubmitInfo, const VkSemaphoreSubmitInfo *waitSemaphoreSubmitInfo) {
		VkSubmitInfo2 info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		info.waitSemaphoreInfoCount = waitSemaphoreSubmitInfo ? 1 : 0;
		info.pWaitSemaphoreInfos = waitSemaphoreSubmitInfo;
		info.signalSemaphoreInfoCount = signalSemaphoreSubmitInfo ? 1 : 0;
		info.pSignalSemaphoreInfos = signalSemaphoreSubmitInfo;
		info.commandBufferInfoCount = 1;
		info.pCommandBufferInfos = commandBufferSubmitInfo;
		return info;
	}

	VkImageCreateInfo imageCreateInfo(const VkFormat format, const VkExtent3D extent, const VkImageUsageFlags usage) {
		VkImageCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = format;
		info.extent = extent;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = usage;
		return info;
	}

	VkImageViewCreateInfo imageViewCreateInfo(const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		info.image = image;
		info.format = format;
		info.subresourceRange.baseMipLevel = 0;
		info.subresourceRange.levelCount = 1;
		info.subresourceRange.baseArrayLayer = 0;
		info.subresourceRange.layerCount = 1;
		info.subresourceRange.aspectMask = aspectFlags;
		return info;
	}
}
