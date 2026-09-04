#pragma once

#include <memory>

#include "glm/mat4x4.hpp"

#include "common/ecs/System.h"
#include "engine/DescriptorAllocator.h"
#include "engine/RenderPipeline.h"

namespace voxel_game::client::render {
	struct ChunkVertex {
		glm::vec3 position;
		uint32_t colour;
	};

	struct ChunkPushConstants {
		glm::mat4 viewProj;
		uint64_t vertexBufferAddress;
	};

	class ChunkRenderer : public ecs::System<ChunkRenderer> {
	public:
		explicit ChunkRenderer(ecs::ECSRegistry& registry);

		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<engine::RenderPipeline> mPipeline = nullptr;
		std::unique_ptr<engine::DescriptorAllocator> mDescriptorAllocator = nullptr;
		std::unique_ptr<engine::DescriptorSet> mDescriptorSet = nullptr;
		std::unique_ptr<engine::GPUBuffer> mVertexBuffer = nullptr;
		ChunkPushConstants mPushConstants = {};
	};
}
