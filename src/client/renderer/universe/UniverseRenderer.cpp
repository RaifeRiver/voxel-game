#include "UniverseRenderer.h"

#include <cmath>
#include <iostream>

#include "glm/gtc/matrix_access.hpp"
#include "tracy/TracyVulkan.hpp"

#include "client/VoxelGameClient.h"
#include "client/renderer/DescriptorSetLayoutBuilder.h"
#include "client/renderer/RenderContext.h"
#include "client/renderer/VulkanImage.h"
#include "client/renderer/VulkanInitialisers.h"
#include "client/renderer/VulkanUtil.h"
#include "client/resource/ShaderResource.h"

namespace voxel_game::client::renderer::universe {
	void UniverseRenderer::init(RenderContext *renderContext) {
		std::unique_lock lock(mChunkIDsMutex);
		mUnusedChunkIDs.reserve(8192);
		for (int i = 8191; i >= 0; i--) {
			mUnusedChunkIDs.push_back(i);
		}
		lock.unlock();

		initBuffers(renderContext);
		initImages(renderContext);
		initDescriptorSets(renderContext);
		initPipelines(renderContext);
	}

	uint32_t UniverseRenderer::getUnusedChunkID() {
		std::unique_lock lock(mChunkIDsMutex);
		uint32_t id = mUnusedChunkIDs.back();
		mUnusedChunkIDs.pop_back();
		return id;
	}

	void UniverseRenderer::unloadChunk(const uint32_t id) {
		std::unique_lock lock(mChunkIDsMutex);
		mUnusedChunkIDs.push_back(id);
	}

	void UniverseRenderer::render(const RenderContext *renderContext, const VkCommandBuffer commandBuffer) {
		/*vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mGradientPipeline);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mGradientPipelineLayout, 0, 1, &renderContext->getDrawImageDescriptorSet(), 0, nullptr);

		GradientPushConstants pushConstants = {};
		auto frame = static_cast<float>(renderContext->getCurrentFrame());
		pushConstants.r = std::abs(std::sin(frame / 900.0f));
		pushConstants.g = std::abs(std::sin((frame + 300.0f) / 900.0f));
		pushConstants.b = std::abs(std::sin((frame + 600.0f) / 900.0f));
		vkCmdPushConstants(commandBuffer, mGradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(GradientPushConstants), &pushConstants);

		vkCmdDispatch(commandBuffer, std::ceil(static_cast<float>(renderContext->getSwapchainExtent().width) / 16.0f), std::ceil(static_cast<float>(renderContext->getSwapchainExtent().height) / 16.0f), 1);*/

		vkCmdFillBuffer(commandBuffer, mDispatchBuffer.buffer, 8, 4, 0);
		vkCmdFillBuffer(commandBuffer, mDispatchBuffer.buffer, 16, 4, 0);
		vkCmdFillBuffer(commandBuffer, mVisibilityBuffer.buffer, 0, VK_WHOLE_SIZE, 0);

		VkBufferMemoryBarrier clearBufferBarriers[2] = {};

		clearBufferBarriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		clearBufferBarriers[0].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		clearBufferBarriers[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		clearBufferBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		clearBufferBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		clearBufferBarriers[0].buffer = mDispatchBuffer.buffer;
		clearBufferBarriers[0].size = VK_WHOLE_SIZE;

		clearBufferBarriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		clearBufferBarriers[1].srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		clearBufferBarriers[1].dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		clearBufferBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		clearBufferBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		clearBufferBarriers[1].buffer = mVisibilityBuffer.buffer;
		clearBufferBarriers[1].size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 2, clearBufferBarriers, 0, nullptr);

		Camera *camera = VoxelGameClient::getClient()->getCamera();
		glm::vec3 cameraPos = camera->position;
		mChunkSelectionPushConstants.cameraPos = cameraPos;
		glm::mat4 view = camera->getViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), static_cast<float>(renderContext->getSwapchainExtent().width) / static_cast<float>(renderContext->getSwapchainExtent().height), 0.1f, 10000.0f);
		projection[1][1] *= -1;
		glm::mat4 viewProj = projection * view;
		getFrustumPlanes(viewProj, mChunkSelectionPushConstants.frustumPlanes);
		mChunkSelectionPushConstants.lodQuality = 4.0f;
		mChunkSelectionPushConstants.chunkCount = mChunks.size();

		{
			TracyVkZone(renderContext->getTracyContext(), commandBuffer, "Chunk selection");

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mChunkSelectionPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mChunkSelectionPipelineLayout, 0, 1, &mChunkSelectionDescriptorSet, 0, nullptr);

			vkCmdPushConstants(commandBuffer, mChunkSelectionPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ChunkSelectionPushConstants), &mChunkSelectionPushConstants);

			vkCmdDispatch(commandBuffer, (mChunks.size() + 63) / 64, 1, 1);
		}

		VkBufferMemoryBarrier tileSelectionBufferBarriers[2] = {};

		tileSelectionBufferBarriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		tileSelectionBufferBarriers[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		tileSelectionBufferBarriers[0].dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		tileSelectionBufferBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		tileSelectionBufferBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		tileSelectionBufferBarriers[0].buffer = mDispatchBuffer.buffer;
		tileSelectionBufferBarriers[0].size = VK_WHOLE_SIZE;

		tileSelectionBufferBarriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		tileSelectionBufferBarriers[1].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		tileSelectionBufferBarriers[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		tileSelectionBufferBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		tileSelectionBufferBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		tileSelectionBufferBarriers[1].buffer = mVisibleChunkBuffer.buffer;
		tileSelectionBufferBarriers[1].size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 2, tileSelectionBufferBarriers, 0, nullptr);

		mTileSelectionPushConstants.cameraPos = cameraPos;
		glm::uvec2 viewportSize = {renderContext->getSwapchainExtent().width, renderContext->getSwapchainExtent().height};
		mTileSelectionPushConstants.viewportSize = viewportSize;
		mTileSelectionPushConstants.viewProj = viewProj;

		{
			TracyVkZone(renderContext->getTracyContext(), commandBuffer, "Tile selection");

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mTileSelectionPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mTileSelectionPipelineLayout, 0, 1, &mTileSelectionDescriptorSet, 0, nullptr);

			vkCmdPushConstants(commandBuffer, mTileSelectionPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(TileSelectionPushConstants), &mTileSelectionPushConstants);

			vkCmdDispatchIndirect(commandBuffer, mDispatchBuffer.buffer, 0);
		}

		VkBufferMemoryBarrier rayMarchBufferBarriers[2] = {};

		rayMarchBufferBarriers[0].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		rayMarchBufferBarriers[0].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		rayMarchBufferBarriers[0].dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		rayMarchBufferBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		rayMarchBufferBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		rayMarchBufferBarriers[0].buffer = mDispatchBuffer.buffer;
		rayMarchBufferBarriers[0].size = VK_WHOLE_SIZE;

		rayMarchBufferBarriers[1].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		rayMarchBufferBarriers[1].srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		rayMarchBufferBarriers[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		rayMarchBufferBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		rayMarchBufferBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		rayMarchBufferBarriers[1].buffer = mTileBuffer.buffer;
		rayMarchBufferBarriers[1].size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 2, rayMarchBufferBarriers, 0, nullptr);

		mRayMarchPushConstants.pos = cameraPos;
		mRayMarchPushConstants.viewportSize = viewportSize;
		mRayMarchPushConstants.invViewProj = glm::inverse(viewProj);
		mRayMarchPushConstants.farPlane = 10000.0f;

		{
			TracyVkZone(renderContext->getTracyContext(), commandBuffer, "Ray marching");

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mRayMarchPipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mRayMarchPipelineLayout, 0, 1, &mRayMarchDescriptorSet, 0, nullptr);

			vkCmdPushConstants(commandBuffer, mRayMarchPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(RayMarchPushConstants), &mRayMarchPushConstants);

			vkCmdDispatchIndirect(commandBuffer, mDispatchBuffer.buffer, 16);
		}

		VkBufferMemoryBarrier colourResolveBufferBarrier = {};
		colourResolveBufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		colourResolveBufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		colourResolveBufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		colourResolveBufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		colourResolveBufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		colourResolveBufferBarrier.buffer = mVisibilityBuffer.buffer;
		colourResolveBufferBarrier.size = VK_WHOLE_SIZE;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0, 0, nullptr, 1, &colourResolveBufferBarrier, 0, nullptr);

		mColourResolvePushConstants.width = viewportSize.x;

		{
			TracyVkZone(renderContext->getTracyContext(), commandBuffer, "Colour resolving");

			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mColourResolvePipeline);
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, mColourResolvePipelineLayout, 0, 1, &mColourResolveDescriptorSet, 0, nullptr);

			vkCmdPushConstants(commandBuffer, mColourResolvePipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ColourResolvePushConstants), &mColourResolvePushConstants);

			vkCmdDispatch(commandBuffer, (viewportSize.x + 15) / 16, (viewportSize.y + 15) / 16, 1);
		}
	}

	void UniverseRenderer::destroy(const RenderContext *renderContext) const {
		vkDeviceWaitIdle(renderContext->getDevice());

		mSVDAGBuffer->destroy(renderContext);
		mMaterialIndexBuffer->destroy(renderContext);
		destroyBuffer(renderContext, mMaterialBuffer);
		destroyBuffer(renderContext, mTileBuffer);
		destroyBuffer(renderContext, mVisibilityBuffer);
		destroyBuffer(renderContext, mChunkBuffer);
		destroyBuffer(renderContext, mVisibleChunkBuffer);
		destroyBuffer(renderContext, mDispatchBuffer);
		destroyBuffer(renderContext, mModelBuffer);
		destroyBuffer(renderContext, mChunkMetadataBuffer);

		vkDestroySampler(renderContext->getDevice(), mZPyramidSampler, nullptr);
		for (const VkImageView& imageView: mZPyramidImageViews) {
			vkDestroyImageView(renderContext->getDevice(), imageView, nullptr);
		}
		destroyImage(renderContext, mZPyramidImage);

		mDescriptorAllocator.destroy(renderContext->getDevice());

		vkDestroyDescriptorSetLayout(renderContext->getDevice(), mChunkSelectionDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(renderContext->getDevice(), mTileSelectionDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(renderContext->getDevice(), mRayMarchDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(renderContext->getDevice(), mColourResolveDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(renderContext->getDevice(), mZPyramidBuilderDescriptorSetLayout, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mGradientPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mGradientPipeline, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mChunkSelectionPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mChunkSelectionPipeline, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mTileSelectionPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mTileSelectionPipeline, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mRayMarchPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mRayMarchPipeline, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mColourResolvePipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mColourResolvePipeline, nullptr);

		vkDestroyPipelineLayout(renderContext->getDevice(), mZPyramidBuilderPipelineLayout, nullptr);
		vkDestroyPipeline(renderContext->getDevice(), mZPyramidBuilderPipeline, nullptr);
	}

	void UniverseRenderer::initBuffers(RenderContext *renderContext) {
		mSVDAGBuffer = std::make_unique<AllocatableBuffer>(renderContext, 512 * 1024 * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
		mMaterialIndexBuffer = std::make_unique<AllocatableBuffer>(renderContext, 64 * 1024 * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		mMaterialBuffer = createBuffer(renderContext, 16 * 1024 * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY, 0);
		mTileBuffer = createBuffer(renderContext, 16 * 1024 * 1024, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, 0);
		VkExtent3D drawImageExtent = renderContext->getDrawImage().extent;
		mVisibilityBuffer = createBuffer(renderContext, drawImageExtent.width * drawImageExtent.height * sizeof(uint64_t), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT);
		mChunkBuffer = createBuffer(renderContext, 48 * 8192, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		mVisibleChunkBuffer = createBuffer(renderContext, 8 * 8192, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, 0);
		mDispatchBuffer = createBuffer(renderContext, 28, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		mModelBuffer = createBuffer(renderContext, 128 * 8192, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
		mChunkMetadataBuffer = createBuffer(renderContext, 16 * 8192, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);

		auto dispatchData = static_cast<uint32_t*>(mDispatchBuffer.allocationInfo.pMappedData);
		// tile_selection dispatch (0-11)
		dispatchData[0] = (drawImageExtent.width + 7) / 8;
		dispatchData[1] = (drawImageExtent.height + 7) / 8;
		dispatchData[2] = 0; // Filled by chunk_selection.comp

		// ray_march dispatch (16-27)
		dispatchData[4] = 0; // Filled by tile_selection.comp
		dispatchData[5] = 1;
		dispatchData[6] = 1;
	}

	void UniverseRenderer::initImages(RenderContext *renderContext) {
		VkExtent3D drawImageExtent = renderContext->getDrawImage().extent;
		mZPyramidImage = createImage(renderContext, drawImageExtent, VK_FORMAT_R32_SFLOAT, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VMA_MEMORY_USAGE_AUTO, 0, true);

		uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(drawImageExtent.width, drawImageExtent.height)))) + 1;
		mZPyramidImageViews.resize(mipLevels, {});
		for (uint32_t i = 0; i < mipLevels; i++) {
			VkImageViewCreateInfo imageViewInfo = initialisers::imageViewCreateInfo(mZPyramidImage.image, VK_FORMAT_R32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);
			imageViewInfo.subresourceRange.baseMipLevel = i;
			imageViewInfo.subresourceRange.levelCount = 1;
			imageViewInfo.subresourceRange.baseArrayLayer = 0;
			imageViewInfo.subresourceRange.layerCount = 1;
			VK_CHECK(vkCreateImageView(renderContext->getDevice(), &imageViewInfo, nullptr, &mZPyramidImageViews[i]));
		}

		VkSamplerReductionModeCreateInfo reductionModeCreateInfo = {};
		reductionModeCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
		reductionModeCreateInfo.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = &reductionModeCreateInfo;
		samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		VK_CHECK(vkCreateSampler(renderContext->getDevice(), &samplerCreateInfo, nullptr, &mZPyramidSampler));

		renderContext->immediateSubmit([this](const VkCommandBuffer commandBuffer) {
			image::transitionImage(commandBuffer, mZPyramidImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});
	}

	void UniverseRenderer::initDescriptorSets(RenderContext *renderContext) {
		std::vector<DescriptorPoolSizeRatio> poolSizes = {
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
		};
		mDescriptorAllocator.init(renderContext->getDevice(), 5, poolSizes);

		VkDescriptorBufferInfo chunkBufferInfo = {};
		chunkBufferInfo.buffer = mChunkBuffer.buffer;
		chunkBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo visibleChunkBufferInfo = {};
		visibleChunkBufferInfo.buffer = mVisibleChunkBuffer.buffer;
		visibleChunkBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorImageInfo zPyramidImageInfo = {};
		zPyramidImageInfo.imageView = mZPyramidImage.imageView;
		zPyramidImageInfo.sampler = mZPyramidSampler;
		zPyramidImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorBufferInfo tileBufferInfo = {};
		tileBufferInfo.buffer = mTileBuffer.buffer;
		tileBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo dispatchBufferInfo1 = {};
		dispatchBufferInfo1.buffer = mDispatchBuffer.buffer;
		dispatchBufferInfo1.range = 12;

		VkDescriptorBufferInfo dispatchBufferInfo2 = {};
		dispatchBufferInfo2.buffer = mDispatchBuffer.buffer;
		dispatchBufferInfo2.offset = 16;
		dispatchBufferInfo2.range = 12;

		VkDescriptorBufferInfo SVDAGBufferInfo = {};
		SVDAGBufferInfo.buffer = mSVDAGBuffer->getBuffer().buffer;
		SVDAGBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo modelBufferInfo = {};
		modelBufferInfo.buffer = mModelBuffer.buffer;
		modelBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo visibilityBufferInfo = {};
		visibilityBufferInfo.buffer = mVisibilityBuffer.buffer;
		visibilityBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo chunkMetadataBufferInfo = {};
		chunkMetadataBufferInfo.buffer = mChunkMetadataBuffer.buffer;
		chunkMetadataBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorBufferInfo materialIndexBufferInfo = {};
		materialIndexBufferInfo.buffer = mMaterialIndexBuffer->getBuffer().buffer;
		materialIndexBufferInfo.range = VK_WHOLE_SIZE;

		VkDescriptorImageInfo drawImageInfo = {};
		drawImageInfo.imageView = renderContext->getDrawImage().imageView;
		drawImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		VkDescriptorBufferInfo materialBufferInfo = {};
		materialBufferInfo.buffer = mMaterialBuffer.buffer;
		materialBufferInfo.range = VK_WHOLE_SIZE;

		DescriptorSetLayoutBuilder layoutBuilder;
		layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		mChunkSelectionDescriptorSetLayout = layoutBuilder.build(renderContext->getDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
		mChunkSelectionDescriptorSet = mDescriptorAllocator.allocate(renderContext->getDevice(), mChunkSelectionDescriptorSetLayout);

		VkWriteDescriptorSet chunkSelectionWriteDescriptorSets[3] = {};

		chunkSelectionWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		chunkSelectionWriteDescriptorSets[0].dstSet = mChunkSelectionDescriptorSet;
		chunkSelectionWriteDescriptorSets[0].descriptorCount = 1;
		chunkSelectionWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		chunkSelectionWriteDescriptorSets[0].dstBinding = 0;
		chunkSelectionWriteDescriptorSets[0].pBufferInfo = &chunkBufferInfo;

		chunkSelectionWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		chunkSelectionWriteDescriptorSets[1].dstSet = mChunkSelectionDescriptorSet;
		chunkSelectionWriteDescriptorSets[1].descriptorCount = 1;
		chunkSelectionWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		chunkSelectionWriteDescriptorSets[1].dstBinding = 1;
		chunkSelectionWriteDescriptorSets[1].pBufferInfo = &visibleChunkBufferInfo;

		chunkSelectionWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		chunkSelectionWriteDescriptorSets[2].dstSet = mChunkSelectionDescriptorSet;
		chunkSelectionWriteDescriptorSets[2].descriptorCount = 1;
		chunkSelectionWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		chunkSelectionWriteDescriptorSets[2].dstBinding = 2;
		chunkSelectionWriteDescriptorSets[2].pBufferInfo = &dispatchBufferInfo1;

		vkUpdateDescriptorSets(renderContext->getDevice(), 3, chunkSelectionWriteDescriptorSets, 0, nullptr);

		layoutBuilder.clear();
		layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		layoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		mTileSelectionDescriptorSetLayout = layoutBuilder.build(renderContext->getDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
		mTileSelectionDescriptorSet = mDescriptorAllocator.allocate(renderContext->getDevice(), mTileSelectionDescriptorSetLayout);

		VkWriteDescriptorSet tileSelectionWriteDescriptorSets[5] = {};

		tileSelectionWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tileSelectionWriteDescriptorSets[0].dstSet = mTileSelectionDescriptorSet;
		tileSelectionWriteDescriptorSets[0].descriptorCount = 1;
		tileSelectionWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		tileSelectionWriteDescriptorSets[0].dstBinding = 0;
		tileSelectionWriteDescriptorSets[0].pBufferInfo = &visibleChunkBufferInfo;

		tileSelectionWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tileSelectionWriteDescriptorSets[1].dstSet = mTileSelectionDescriptorSet;
		tileSelectionWriteDescriptorSets[1].descriptorCount = 1;
		tileSelectionWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		tileSelectionWriteDescriptorSets[1].dstBinding = 1;
		tileSelectionWriteDescriptorSets[1].pImageInfo = &zPyramidImageInfo;

		tileSelectionWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tileSelectionWriteDescriptorSets[2].dstSet = mTileSelectionDescriptorSet;
		tileSelectionWriteDescriptorSets[2].descriptorCount = 1;
		tileSelectionWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		tileSelectionWriteDescriptorSets[2].dstBinding = 2;
		tileSelectionWriteDescriptorSets[2].pBufferInfo = &tileBufferInfo;

		tileSelectionWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tileSelectionWriteDescriptorSets[3].dstSet = mTileSelectionDescriptorSet;
		tileSelectionWriteDescriptorSets[3].descriptorCount = 1;
		tileSelectionWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		tileSelectionWriteDescriptorSets[3].dstBinding = 3;
		tileSelectionWriteDescriptorSets[3].pBufferInfo = &dispatchBufferInfo2;

		tileSelectionWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		tileSelectionWriteDescriptorSets[4].dstSet = mTileSelectionDescriptorSet;
		tileSelectionWriteDescriptorSets[4].descriptorCount = 1;
		tileSelectionWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		tileSelectionWriteDescriptorSets[4].dstBinding = 4;
		tileSelectionWriteDescriptorSets[4].pBufferInfo = &chunkBufferInfo;

		vkUpdateDescriptorSets(renderContext->getDevice(), 5, tileSelectionWriteDescriptorSets, 0, nullptr);

		layoutBuilder.clear();
		layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		mRayMarchDescriptorSetLayout = layoutBuilder.build(renderContext->getDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
		mRayMarchDescriptorSet = mDescriptorAllocator.allocate(renderContext->getDevice(), mRayMarchDescriptorSetLayout);

		VkWriteDescriptorSet rayMarchWriteDescriptorSets[5] = {};

		rayMarchWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		rayMarchWriteDescriptorSets[0].dstSet = mRayMarchDescriptorSet;
		rayMarchWriteDescriptorSets[0].descriptorCount = 1;
		rayMarchWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		rayMarchWriteDescriptorSets[0].dstBinding = 0;
		rayMarchWriteDescriptorSets[0].pBufferInfo = &tileBufferInfo;

		rayMarchWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		rayMarchWriteDescriptorSets[1].dstSet = mRayMarchDescriptorSet;
		rayMarchWriteDescriptorSets[1].descriptorCount = 1;
		rayMarchWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		rayMarchWriteDescriptorSets[1].dstBinding = 1;
		rayMarchWriteDescriptorSets[1].pBufferInfo = &SVDAGBufferInfo;

		rayMarchWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		rayMarchWriteDescriptorSets[2].dstSet = mRayMarchDescriptorSet;
		rayMarchWriteDescriptorSets[2].descriptorCount = 1;
		rayMarchWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		rayMarchWriteDescriptorSets[2].dstBinding = 2;
		rayMarchWriteDescriptorSets[2].pBufferInfo = &modelBufferInfo;

		rayMarchWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		rayMarchWriteDescriptorSets[3].dstSet = mRayMarchDescriptorSet;
		rayMarchWriteDescriptorSets[3].descriptorCount = 1;
		rayMarchWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		rayMarchWriteDescriptorSets[3].dstBinding = 3;
		rayMarchWriteDescriptorSets[3].pBufferInfo = &visibilityBufferInfo;

		rayMarchWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		rayMarchWriteDescriptorSets[4].dstSet = mRayMarchDescriptorSet;
		rayMarchWriteDescriptorSets[4].descriptorCount = 1;
		rayMarchWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		rayMarchWriteDescriptorSets[4].dstBinding = 4;
		rayMarchWriteDescriptorSets[4].pBufferInfo = &chunkMetadataBufferInfo;

		vkUpdateDescriptorSets(renderContext->getDevice(), 5, rayMarchWriteDescriptorSets, 0, nullptr);

		layoutBuilder.clear();
		layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 64);
		layoutBuilder.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		layoutBuilder.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		mColourResolveDescriptorSetLayout = layoutBuilder.build(renderContext->getDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
		mColourResolveDescriptorSet = mDescriptorAllocator.allocate(renderContext->getDevice(), mColourResolveDescriptorSetLayout);

		VkWriteDescriptorSet colourResolveWriteDescriptorSets[8] = {};

		colourResolveWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[0].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[0].descriptorCount = 1;
		colourResolveWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[0].dstBinding = 0;
		colourResolveWriteDescriptorSets[0].pBufferInfo = &visibilityBufferInfo;

		colourResolveWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[1].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[1].descriptorCount = 1;
		colourResolveWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[1].dstBinding = 1;
		colourResolveWriteDescriptorSets[1].pBufferInfo = &materialIndexBufferInfo;

		colourResolveWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[2].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[2].descriptorCount = 1;
		colourResolveWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[2].dstBinding = 2;
		colourResolveWriteDescriptorSets[2].pBufferInfo = &chunkMetadataBufferInfo;

		colourResolveWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[3].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[3].descriptorCount = 64;
		colourResolveWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		colourResolveWriteDescriptorSets[3].dstBinding = 3;
		std::vector<VkDescriptorImageInfo> dummyTextureInfos(64);
		for (int i = 0; i < 64; i++) {
			dummyTextureInfos[i].sampler = mZPyramidSampler;
			dummyTextureInfos[i].imageView = mZPyramidImage.imageView;
			dummyTextureInfos[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		}
		colourResolveWriteDescriptorSets[3].pImageInfo = dummyTextureInfos.data(); // TODO: Create block texture samplers

		colourResolveWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[4].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[4].descriptorCount = 1;
		colourResolveWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		colourResolveWriteDescriptorSets[4].dstBinding = 4;
		colourResolveWriteDescriptorSets[4].pImageInfo = &drawImageInfo;

		colourResolveWriteDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[5].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[5].descriptorCount = 1;
		colourResolveWriteDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[5].dstBinding = 5;
		colourResolveWriteDescriptorSets[5].pBufferInfo = &SVDAGBufferInfo;

		colourResolveWriteDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[6].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[6].descriptorCount = 1;
		colourResolveWriteDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[6].dstBinding = 6;
		colourResolveWriteDescriptorSets[6].pBufferInfo = &materialBufferInfo;

		colourResolveWriteDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		colourResolveWriteDescriptorSets[7].dstSet = mColourResolveDescriptorSet;
		colourResolveWriteDescriptorSets[7].descriptorCount = 1;
		colourResolveWriteDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		colourResolveWriteDescriptorSets[7].dstBinding = 7;
		colourResolveWriteDescriptorSets[7].pBufferInfo = &modelBufferInfo;

		vkUpdateDescriptorSets(renderContext->getDevice(), 7, colourResolveWriteDescriptorSets, 0, nullptr);

		layoutBuilder.clear();
		layoutBuilder.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		layoutBuilder.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		layoutBuilder.addBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		mZPyramidBuilderDescriptorSetLayout = layoutBuilder.build(renderContext->getDevice(), VK_SHADER_STAGE_COMPUTE_BIT);
		mZPyramidBuilderDescriptorSet = mDescriptorAllocator.allocate(renderContext->getDevice(), mZPyramidBuilderDescriptorSetLayout);

		VkWriteDescriptorSet zPyramidBuilderWriteDescriptorSets[3] = {};

		zPyramidBuilderWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		zPyramidBuilderWriteDescriptorSets[0].dstSet = mZPyramidBuilderDescriptorSet;
		zPyramidBuilderWriteDescriptorSets[0].descriptorCount = 1;
		zPyramidBuilderWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		zPyramidBuilderWriteDescriptorSets[0].dstBinding = 0;
		zPyramidBuilderWriteDescriptorSets[0].pBufferInfo = &visibilityBufferInfo;

		zPyramidBuilderWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		zPyramidBuilderWriteDescriptorSets[1].dstSet = mZPyramidBuilderDescriptorSet;
		zPyramidBuilderWriteDescriptorSets[1].descriptorCount = 1;
		zPyramidBuilderWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		zPyramidBuilderWriteDescriptorSets[1].dstBinding = 1;
		zPyramidBuilderWriteDescriptorSets[1].pImageInfo = &dummyTextureInfos[0]; // TODO

		zPyramidBuilderWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		zPyramidBuilderWriteDescriptorSets[2].dstSet = mZPyramidBuilderDescriptorSet;
		zPyramidBuilderWriteDescriptorSets[2].descriptorCount = 1;
		zPyramidBuilderWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		zPyramidBuilderWriteDescriptorSets[2].dstBinding = 2;
		zPyramidBuilderWriteDescriptorSets[2].pImageInfo = &dummyTextureInfos[1]; // TODO

		vkUpdateDescriptorSets(renderContext->getDevice(), 3, zPyramidBuilderWriteDescriptorSets, 0, nullptr);
	}

	void UniverseRenderer::initPipelines(RenderContext *renderContext) {
		createComputePipeline(mGradientPipeline, mGradientPipelineLayout, renderContext->getDevice(), "shaders/gradient.comp.spv", 1, &renderContext->getDrawImageDescriptorSetLayout(), sizeof(GradientPushConstants));

		createComputePipeline(mChunkSelectionPipeline, mChunkSelectionPipelineLayout, renderContext->getDevice(), "shaders/chunk_selection.comp.spv", 1, &mChunkSelectionDescriptorSetLayout, sizeof(ChunkSelectionPushConstants));
		createComputePipeline(mTileSelectionPipeline, mTileSelectionPipelineLayout, renderContext->getDevice(), "shaders/tile_selection.comp.spv", 1, &mTileSelectionDescriptorSetLayout, sizeof(TileSelectionPushConstants));
		createComputePipeline(mRayMarchPipeline, mRayMarchPipelineLayout, renderContext->getDevice(), "shaders/ray_march.comp.spv", 1, &mRayMarchDescriptorSetLayout, sizeof(RayMarchPushConstants));
		createComputePipeline(mColourResolvePipeline, mColourResolvePipelineLayout, renderContext->getDevice(), "shaders/colour_resolve.comp.spv", 1, &mColourResolveDescriptorSetLayout, sizeof(ColourResolvePushConstants));
		createComputePipeline(mZPyramidBuilderPipeline, mZPyramidBuilderPipelineLayout, renderContext->getDevice(), "shaders/zpyramid_builder.comp.spv", 1, &mZPyramidBuilderDescriptorSetLayout, sizeof(ZPyramidBuilderPushConstants));
	}

	void UniverseRenderer::getFrustumPlanes(const glm::mat4 &view, glm::vec4 planes[6]) {
		glm::vec4 row1 = glm::row(view, 0);
		glm::vec4 row2 = glm::row(view, 1);
		glm::vec4 row3 = glm::row(view, 2);
		glm::vec4 row4 = glm::row(view, 3);

		planes[0] = row4 + row1;
		planes[1] = row4 - row1;
		planes[2] = row4 + row2;
		planes[3] = row4 - row2;
		planes[4] = row3;
		planes[5] = row4 - row3;

		for (int i = 0; i < 6; i++) {
			float length = glm::length(glm::vec3(planes[i]));
			planes[i] /= length;
		}
	}

	void UniverseRenderer::createComputePipeline(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout, const VkDevice device, const std::string &path, const uint32_t descriptorSetLayoutCount, const VkDescriptorSetLayout *descriptorSetLayouts, const uint32_t pushConstantSize) {
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = descriptorSetLayoutCount;
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.size = pushConstantSize;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

		auto *shader = VoxelGameClient::getClient()->getResourceManager()->load<resource::ShaderResource>(path, path);
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		shaderStageCreateInfo.module = shader->getShaderModule();
		shaderStageCreateInfo.pName = "main";

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stage = shaderStageCreateInfo;
		pipelineCreateInfo.layout = pipelineLayout;
		VK_CHECK(vkCreateComputePipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));

		VoxelGameClient::getClient()->getResourceManager()->unload(path);
	}
}
