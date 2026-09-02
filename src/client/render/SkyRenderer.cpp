#include "SkyRenderer.h"

#include "engine/RenderEngine.h"
#include "common/resource/ResourceManager.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render {
	SkyRenderer::SkyRenderer(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<engine::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		const std::string vertexShader = util::readFileAsString(resourceManager.findResource("voxel_game:shaders/sky", ".vert", resource::ResourceType::ASSET).path);
		const std::string fragmentShader = util::readFileAsString(resourceManager.findResource("voxel_game:shaders/sky", ".frag", resource::ResourceType::ASSET).path);
		mPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->build();
	}

	void SkyRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, const float) {
		if (stage != ecs::SystemStage::BACKGROUND_RENDER) {
			return;
		}

		auto& renderEngine = registry.getResource<engine::RenderEngine>();

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->draw(6);

		renderEngine.endRendering();
	}
}
