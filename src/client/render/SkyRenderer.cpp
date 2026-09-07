/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
