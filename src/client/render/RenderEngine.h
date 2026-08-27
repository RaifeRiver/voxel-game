#pragma once

#include <memory>
#include <string>

#include "glm/vec3.hpp"

#include "ComputePipeline.h"
#include "DescriptorAllocator.h"
#include "GPUImage.h"
#include "RenderPipeline.h"
#include "common/ecs/Resource.h"

namespace voxel_game::client::render {
	constexpr uint32_t FRAME_OVERLAP = 2;

	class RenderEngine : public ecs::Resource<RenderEngine> {
	public:
		virtual std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) = 0;

		virtual std::unique_ptr<ComputePipeline> createComputePipeline(const std::string& computeShader) = 0;

		virtual std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) = 0;

		virtual std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() = 0;

		virtual GPUImage& getRenderImage() = 0;

		virtual void waitForGPU() = 0;

		[[nodiscard]] uint64_t getFrame() const {
			return mFrame;
		}

	protected:
		uint64_t mFrame = 0;
	};
}
