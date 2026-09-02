#pragma once

#include "engine/RenderEngine.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render {
	class SkyRenderer : public ecs::System<SkyRenderer> {
	public:
		explicit SkyRenderer(ecs::ECSRegistry& registry);

		void runStage(ecs::SystemStage stage, ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<engine::RenderPipeline> mPipeline = nullptr;
	};
}
