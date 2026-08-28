#pragma once

#include "client/render/DescriptorAllocator.h"
#include "client/render/RenderEngine.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::system {
	class SkyRenderSystem : public ecs::System<SkyRenderSystem> {
	public:
		explicit SkyRenderSystem(ecs::ECSRegistry& registry);

		void runStage(ecs::Stage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<render::RenderPipeline> mPipeline = nullptr;
	};
}
