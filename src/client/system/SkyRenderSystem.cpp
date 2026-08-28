#include "SkyRenderSystem.h"

#include "client/render/RenderEngine.h"
#include "common/resource/ResourceManager.h"

namespace voxel_game::client::system {
	SkyRenderSystem::SkyRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		const std::string vertexShader = resourceManager.findResource("voxel_game:shaders/sky", ".vert.spv", resource::ResourceType::ASSET).path;
		const std::string fragmentShader = resourceManager.findResource("voxel_game:shaders/sky", ".frag.spv", resource::ResourceType::ASSET).path;
		mPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->build();
	}

	void SkyRenderSystem::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, const float) {
		if (stage != ecs::SystemStage::BACKGROUND_RENDER) {
			return;
		}

		auto& renderEngine = registry.getResource<render::RenderEngine>();

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->draw(6);

		renderEngine.endRendering();
	}
}
