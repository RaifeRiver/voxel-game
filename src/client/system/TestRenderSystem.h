#pragma once

#include "client/render/DescriptorAllocator.h"
#include "client/render/RenderPipeline.h"
#include "client/render/RenderEngine.h"
#include "common/ecs/SystemManager.h"

namespace voxel_game::client::system {
	class TestRenderSystem : public ecs::System {
	public:
		explicit TestRenderSystem(ecs::ECSRegistry& registry);

		void render(ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<render::RenderPipeline> mPipeline = nullptr;
		std::unique_ptr<render::DescriptorAllocator> mDescriptorAllocator = nullptr;
		std::unique_ptr<render::DescriptorSet> mDescriptorSets[render::FRAME_OVERLAP];
		std::unique_ptr<render::GPUBuffer> mVertexBuffer = nullptr;
		std::unique_ptr<render::GPUBuffer> mIndexBuffer = nullptr;
	};
}
