#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "SVDAG.h"
#include "glm/mat4x4.hpp"

#include "client/renderer/AllocatableBuffer.h"
#include "client/renderer/RenderContext.h"

namespace voxel_game::client::renderer::universe {
	struct GradientPushConstants {
		float r;
		float g;
		float b;
	};

	struct ChunkSelectionPushConstants {
		glm::vec4 frustumPlanes[6];
		glm::vec3 cameraPos;
		float lodQuality;
		uint32_t chunkCount;
	};

	struct TileSelectionPushConstants {
		glm::mat4 viewProj;
		glm::uvec2 viewportSize;
		alignas(16) glm::vec3 cameraPos;
	};

	struct RayMarchPushConstants {
		glm::mat4 invViewProj;
		glm::vec3 pos;
		alignas(8) glm::uvec2 viewportSize;
		float farPlane;
	};

	struct ColourResolvePushConstants {
		uint32_t width;
	};

	struct ZPyramidBuilderPushConstants {
		glm::uvec2 size;
		uint32_t level;
	};

	struct ChunkMetadata {
		uint32_t rootOffset;
		uint32_t attributeOffset;
		uint32_t attributeCount;
		uint32_t objectID;
	};

	class UniverseRenderer {
	public:
		void init(RenderContext *renderContext);

		[[nodiscard]] AllocatableBuffer *getSVDAGBuffer() const {
			return mSVDAGBuffer.get();
		}

		[[nodiscard]] AllocatableBuffer *getMaterialIndexBuffer() const {
			return mMaterialIndexBuffer.get();
		}

		[[nodiscard]] AllocatedBuffer &getModelBuffer() {
			return mModelBuffer;
		}

		[[nodiscard]] AllocatedBuffer &getChunkMetadataBuffer() {
			return mChunkMetadataBuffer;
		}

		[[nodiscard]] AllocatedBuffer &getChunkBuffer() {
			return mChunkBuffer;
		}

		[[nodiscard]] uint32_t getUnusedChunkID();

		void unloadChunk(uint32_t id);

		void render(const RenderContext *renderContext, VkCommandBuffer commandBuffer);

		[[nodiscard]] std::vector<std::unique_ptr<SVDAG>> &getChunks() {
			return mChunks;
		}

		void destroy(const RenderContext *renderContext) const;

	private:
		DescriptorAllocator mDescriptorAllocator;

		VkPipeline mGradientPipeline = nullptr;
		VkPipelineLayout mGradientPipelineLayout = nullptr;

		VkPipeline mChunkSelectionPipeline = nullptr;
		VkPipelineLayout mChunkSelectionPipelineLayout = nullptr;
		VkDescriptorSet mChunkSelectionDescriptorSet = nullptr;
		VkDescriptorSetLayout mChunkSelectionDescriptorSetLayout = nullptr;

		VkPipeline mTileSelectionPipeline = nullptr;
		VkPipelineLayout mTileSelectionPipelineLayout = nullptr;
		VkDescriptorSet mTileSelectionDescriptorSet = nullptr;
		VkDescriptorSetLayout mTileSelectionDescriptorSetLayout = nullptr;

		VkPipeline mRayMarchPipeline = nullptr;
		VkPipelineLayout mRayMarchPipelineLayout = nullptr;
		VkDescriptorSet mRayMarchDescriptorSet = nullptr;
		VkDescriptorSetLayout mRayMarchDescriptorSetLayout = nullptr;

		VkPipeline mColourResolvePipeline = nullptr;
		VkPipelineLayout mColourResolvePipelineLayout = nullptr;
		VkDescriptorSet mColourResolveDescriptorSet = nullptr;
		VkDescriptorSetLayout mColourResolveDescriptorSetLayout = nullptr;

		VkPipeline mZPyramidBuilderPipeline = nullptr;
		VkPipelineLayout mZPyramidBuilderPipelineLayout = nullptr;
		VkDescriptorSet mZPyramidBuilderDescriptorSet = nullptr;
		VkDescriptorSetLayout mZPyramidBuilderDescriptorSetLayout = nullptr;

		std::unique_ptr<AllocatableBuffer> mSVDAGBuffer = nullptr;
		std::unique_ptr<AllocatableBuffer> mMaterialIndexBuffer = nullptr;
		AllocatedBuffer mMaterialBuffer = {};
		AllocatedBuffer mTileBuffer = {};
		AllocatedBuffer mVisibilityBuffer = {};
		AllocatedBuffer mChunkBuffer = {};
		AllocatedBuffer mVisibleChunkBuffer = {};
		AllocatedBuffer mDispatchBuffer = {};
		AllocatedBuffer mModelBuffer = {};
		AllocatedBuffer mChunkMetadataBuffer = {};

		AllocatedImage mZPyramidImage = {};
		std::vector<VkImageView> mZPyramidImageViews = {};
		VkSampler mZPyramidSampler = nullptr;

		ChunkSelectionPushConstants mChunkSelectionPushConstants = {};
		TileSelectionPushConstants mTileSelectionPushConstants = {};
		RayMarchPushConstants mRayMarchPushConstants = {};
		ColourResolvePushConstants mColourResolvePushConstants = {};
		ZPyramidBuilderPushConstants mZPyramidBuilderPushConstants = {};

		std::vector<std::unique_ptr<SVDAG>> mChunks = {};
		std::mutex mChunkIDsMutex = {};
		std::vector<uint16_t> mUnusedChunkIDs = {};

		void initBuffers(RenderContext *renderContext);

		void initImages(RenderContext * renderContext);

		void initDescriptorSets(RenderContext *renderContext);

		void initPipelines(RenderContext *renderContext);

		static void getFrustumPlanes(const glm::mat4 &view, glm::vec4 planes[6]);

		static void createComputePipeline(VkPipeline &pipeline, VkPipelineLayout &pipelineLayout, VkDevice device, const std::string& path, uint32_t descriptorSetLayoutCount, const VkDescriptorSetLayout *descriptorSetLayouts, uint32_t pushConstantSize);
	};
}
