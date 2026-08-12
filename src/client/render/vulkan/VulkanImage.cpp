#include "VulkanImage.h"

#include <stdexcept>

#include "VulkanEngine.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::vulkan {
	VkFormat toVKImageFormat(const ImageFormat format) {
		switch (format) {
			case ImageFormat::UNKNOWN:
				return VK_FORMAT_UNDEFINED;
			case ImageFormat::RGBA8_SRGB:
				return VK_FORMAT_R8G8B8A8_SRGB;
			case ImageFormat::RGBA16_SFLOAT:
				return VK_FORMAT_R16G16B16A16_SFLOAT;
			case ImageFormat::D32_SFLOAT:
				return VK_FORMAT_D32_SFLOAT;
			default:
				throw std::runtime_error("Unsupported image format");
		}
	}

	VkImageUsageFlagBits toVKImageUsage(const ImageUsage::ImageUsage usage) {
		uint32_t imageUsage = 0;
		if (usage & ImageUsage::TRANSFER_SRC) {
			imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		if (usage & ImageUsage::TRANSFER_DST) {
			imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if (usage & ImageUsage::SAMPLED) {
			imageUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (usage & ImageUsage::STORAGE) {
			imageUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
		}
		if (usage & ImageUsage::COLOUR_ATTACHMENT) {
			imageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if (usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		return static_cast<VkImageUsageFlagBits>(imageUsage);
	}

	VkImageType toVKImageType(const ImageType type) {
		switch (type) {
			case ImageType::IMAGE_1D:
				return VK_IMAGE_TYPE_1D;
			case ImageType::IMAGE_2D:
				return VK_IMAGE_TYPE_2D;
			case ImageType::IMAGE_3D:
				return VK_IMAGE_TYPE_3D;
			default:
				throw std::runtime_error("Unsupported image type");
		}
	}

	VkImageViewType toVKImageViewType(const ImageType type) {
		switch (type) {
			case ImageType::IMAGE_1D:
				return VK_IMAGE_VIEW_TYPE_1D;
			case ImageType::IMAGE_2D:
				return VK_IMAGE_VIEW_TYPE_2D;
			case ImageType::IMAGE_3D:
				return VK_IMAGE_VIEW_TYPE_3D;
			default:
				throw std::runtime_error("Unsupported image type");
		}
	}

	VkImageAspectFlags toVKImageAspectFlags(const ImageFormat format) {
		switch (format) {
			case ImageFormat::UNKNOWN:
			case ImageFormat::RGBA8_SRGB:
			case ImageFormat::RGBA16_SFLOAT:
				return VK_IMAGE_ASPECT_COLOR_BIT;
			case ImageFormat::D32_SFLOAT:
				return VK_IMAGE_ASPECT_DEPTH_BIT;
			default:
				throw std::runtime_error("Unsupported image format");
		}
	}

	VulkanImage::VulkanImage(const VulkanEngine* vulkanEngine, const glm::uvec3 size, const ImageFormat format, const ImageUsage::ImageUsage usage, const ImageType type) : GPUImage(size, format, usage, type) {
		const VkImageCreateInfo imageCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = toVKImageType(type),
			.format = toVKImageFormat(format),
			.extent = {.width = size.x, .height = size.y, .depth = size.z},
			.mipLevels = 1,
			.arrayLayers = 1,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = VK_IMAGE_TILING_OPTIMAL,
			.usage = toVKImageUsage(usage),
		};
		constexpr VmaAllocationCreateInfo allocationCreateInfo = {
			.usage = VMA_MEMORY_USAGE_GPU_ONLY,
			.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};
		vulkan_util::vkCheck(vmaCreateImage(vulkanEngine->getVMAAllocator(), &imageCreateInfo, &allocationCreateInfo, &mImage, &mAllocation, nullptr));

		const VkImageViewCreateInfo imageViewCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = mImage,
			.viewType = toVKImageViewType(type),
			.format = toVKImageFormat(format),
			.subresourceRange = {
				.aspectMask = toVKImageAspectFlags(format),
				.levelCount = 1,
				.layerCount = 1
			}
		};
		vulkan_util::vkCheck(vkCreateImageView(vulkanEngine->getDevice(), &imageViewCreateInfo, nullptr, &mImageView));
	}
}
