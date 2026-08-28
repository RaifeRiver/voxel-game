#include "TestRenderSystem.h"

#include "common/ecs/ECSRegistry.h"
#include "common/resource/ResourceManager.h"

namespace voxel_game::client::system {
	TestRenderSystem::TestRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		mPipeline = renderEngine.createRenderPipelineBuilder(resourceManager.findResource("voxel_game:shaders/test", ".vert.spv", resource::ResourceType::ASSET).path, resourceManager.findResource("voxel_game:shaders/test", ".frag.spv", resource::ResourceType::ASSET).path)->build();
	}

	void TestRenderSystem::render(ecs::ECSRegistry& registry, float) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->draw(3);

		renderEngine.endRendering();
	}
}
