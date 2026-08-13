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

	VkImageUsageFlagBits toVKImageUsage(const ImageUsage usage) {
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
		if (usage & ImageUsage::DEPTH_ATTACHMENT) {
			imageUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		return static_cast<VkImageUsageFlagBits>(imageUsage);
	}

	VkImageLayout toVKImageLayout(const ImageUsage usage) {
		if (usage & ImageUsage::TRANSFER_SRC) {
			return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		}
		if (usage & ImageUsage::TRANSFER_DST) {
			return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		}
		if (usage & ImageUsage::SAMPLED) {
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		if (usage & ImageUsage::STORAGE) {
			return VK_IMAGE_LAYOUT_GENERAL;
		}
		if (usage & ImageUsage::COLOUR_ATTACHMENT) {
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		if (usage & ImageUsage::DEPTH_ATTACHMENT) {
			return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		}
		throw std::runtime_error("Unsupported image usage");
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

	VulkanImage::VulkanImage(VulkanEngine* vulkanEngine, const glm::uvec3 size, const ImageFormat format, const ImageUsage usage, const ImageType type) : GPUImage(size, format, usage, type), mVulkanEngine(vulkanEngine) {
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

	void VulkanImage::transition(const ImageUsage usage) {
		if (usage != mCurrentUsage) {
			if ((usage & mUsage) == static_cast<uint32_t>(mUsage)) {
				throw std::runtime_error("Image must be created with usage");
			}
			const VkImageLayout layout = toVKImageLayout(usage);
			transitionImage(mVulkanEngine->getCommandBuffer(), mImage, mCurrentLayout, layout);
			mCurrentLayout = layout;
		}
	}

	void VulkanImage::clearColour(const glm::vec4 colour) {
		if (!(mUsage & ImageUsage::TRANSFER_DST)) {
			throw std::runtime_error("Image must have ImageUsage::TRANSFER_DST");
		}
		if (mCurrentUsage != ImageUsage::TRANSFER_DST && mCurrentUsage != ImageUsage::STORAGE) {
			transition(ImageUsage::TRANSFER_DST);
		}

		const VkClearColorValue clearColorValue = {colour.x, colour.y, colour.z, colour.a};
		constexpr VkImageSubresourceRange subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = VK_REMAINING_MIP_LEVELS,
			.baseArrayLayer = 0,
			.layerCount = VK_REMAINING_ARRAY_LAYERS,
		};
		vkCmdClearColorImage(mVulkanEngine->getCommandBuffer(), mImage, mCurrentLayout, &clearColorValue, 1, &subresourceRange);
	}

	VulkanImage::~VulkanImage() {
		vkDestroyImageView(mVulkanEngine->getDevice(), mImageView, nullptr);
		vmaDestroyImage(mVulkanEngine->getVMAAllocator(), mImage, mAllocation);
	}

	// ReSharper disable CppParameterMayBeConst
	void transitionImage(VkCommandBuffer commandBuffer, VkImage image, const VkImageLayout currentLayout, const VkImageLayout newLayout) {
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

	void copyImage(VkCommandBuffer commandBuffer, VkImage src, VkImage dst, const glm::uvec3 srcSize, const glm::uvec3 dstSize) {
		const VkImageBlit2 imageBlit = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.layerCount = 1
			},
			.srcOffsets = {{}, {.x = static_cast<int>(srcSize.x), .y = static_cast<int>(srcSize.y), .z = static_cast<int>(srcSize.z)}},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.layerCount = 1
			},
			.dstOffsets = {{}, {.x = static_cast<int>(dstSize.x), .y = static_cast<int>(dstSize.y), .z = static_cast<int>(dstSize.z)}}
		};
		const VkBlitImageInfo2 blitImageInfo = {
			.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
			.srcImage = src,
			.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			.dstImage = dst,
			.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.regionCount = 1,
			.pRegions = &imageBlit,
			.filter = VK_FILTER_LINEAR
		};
		vkCmdBlitImage2(commandBuffer, &blitImageInfo);
	}
	// ReSharper restore CppParameterMayBeConst
}
