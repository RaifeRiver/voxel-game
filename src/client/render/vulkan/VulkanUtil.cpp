#include "VulkanUtil.h"

namespace voxel_game::client::render::vulkan {
	void vulkan_util::transitionImage(const VkCommandBuffer commandBuffer, const VkImage image, const VkImageLayout currentLayout, const VkImageLayout newLayout) {
		const VkImageSubresourceRange subresourceRange = {
			.aspectMask = newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS,
		};
		VkImageMemoryBarrier2 imageMemoryBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
			.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
			.oldLayout = currentLayout,
			.newLayout = newLayout,
			.image = image,
			.subresourceRange = subresourceRange
		};

		const VkDependencyInfo dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &imageMemoryBarrier,
		};
		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
	}
}
