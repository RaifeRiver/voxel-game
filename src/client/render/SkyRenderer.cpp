#include "SkyRenderer.h"

#include "tracy/Tracy.hpp"

#include "engine/RenderEngine.h"
#include "common/resource/ResourceManager.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render {
	SkyRenderer::SkyRenderer(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init sky renderer");

		auto& renderEngine = registry.getResource<engine::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		const engine::Shader vertexShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/sky", ".vert", resource::ResourceType::ASSET).path).build(engine::ShaderStage::VERTEX);
		const engine::Shader fragmentShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/sky", ".frag", resource::ResourceType::ASSET).path).build(engine::ShaderStage::FRAGMENT);
		mPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->build();
	}

	void SkyRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, const float) {
		if (stage != ecs::SystemStage::BACKGROUND_RENDER) {
			return;
		}

		ZoneScopedN("Render sky");

		auto& renderEngine = registry.getResource<engine::RenderEngine>();

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->draw(6, 0, "Render sky");

		renderEngine.endRendering();
	}
}
