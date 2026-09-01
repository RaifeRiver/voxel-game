#pragma once

#include "../render/engine/DescriptorAllocator.h"
#include "../render/engine/RenderEngine.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::system {
	class SkyRenderSystem : public ecs::System<SkyRenderSystem> {
	public:
		explicit SkyRenderSystem(ecs::ECSRegistry& registry);

		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<render::RenderPipeline> mPipeline = nullptr;
	};
}
