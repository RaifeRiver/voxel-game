#include "TestRenderSystem.h"

#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::system {
	TestRenderSystem::TestRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();

		mPipeline = renderEngine.createRenderPipelineBuilder("res/assets/voxel_game/shaders/test.vert.spv", "res/assets/voxel_game/shaders/test.frag.spv")->build();
	}

	void TestRenderSystem::render(ecs::ECSRegistry& registry, float) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->draw(3);

		renderEngine.endRendering();
	}
}
