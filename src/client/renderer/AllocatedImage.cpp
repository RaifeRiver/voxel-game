#include "AllocatedImage.h"

#include <cmath>

#include "RenderContext.h"
#include "VulkanInitialisers.h"
#include "VulkanUtil.h"

namespace voxel_game::client::renderer {
	AllocatedImage createImage(const RenderContext *renderContext, const VkExtent3D extent, const VkFormat format, const VkImageUsageFlags usage, const VmaMemoryUsage memoryUsage, const VmaAllocationCreateFlags flags, const bool mipmapped) {
		AllocatedImage image = {};
		image.format = format;
		image.extent = extent;

		VkImageCreateInfo imageInfo = initialisers::imageCreateInfo(format, extent, usage);
		if (mipmapped) {
			imageInfo.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1;
		}

		VmaAllocationCreateInfo allocationInfo = {};
		allocationInfo.usage = memoryUsage;
		allocationInfo.flags = flags;
		allocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		VK_CHECK(vmaCreateImage(renderContext->getAllocator(), &imageInfo, &allocationInfo, &image.image, &image.allocation, nullptr));

		VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT) {
			aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		VkImageViewCreateInfo imageViewInfo = initialisers::imageViewCreateInfo(image.image, format, aspectFlags);
		imageViewInfo.subresourceRange.levelCount = imageInfo.mipLevels;
		VK_CHECK(vkCreateImageView(renderContext->getDevice(), &imageViewInfo, nullptr, &image.imageView));

		return image;
	}

	void destroyImage(const RenderContext *renderContext, const AllocatedImage &image) {
		vkDestroyImageView(renderContext->getDevice(), image.imageView, nullptr);
		vmaDestroyImage(renderContext->getAllocator(), image.image, image.allocation);
	}
}
