#include "BackgroundRenderSystem.h"

#include "client/render/RenderEngine.h"
#include "client/render/Shader.h"
#include "client/window/Window.h"

namespace voxel_game::client::system {
	BackgroundRenderSystem::BackgroundRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();

		mPipeline = renderEngine.createComputePipeline("res/assets/voxel_game/shaders/gradient.comp.spv");

		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->addBinding(0, render::DescriptorType::IMAGE)->build(render::FRAME_OVERLAP, render::ShaderStage::COMPUTE);

		for (std::unique_ptr<render::DescriptorSet>& descriptorSet : mDescriptorSets) {
			descriptorSet = mDescriptorAllocator->allocate();
		}
	}

	void BackgroundRenderSystem::backgroundRender(ecs::ECSRegistry& registry, float) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		auto& window = registry.getResource<window::Window>();

		const glm::uvec2 windowSize = window.getSize();

		render::DescriptorSet* descriptorSet = mDescriptorSets[renderEngine.getFrame() % render::FRAME_OVERLAP].get();
		descriptorSet->setBinding(0, &renderEngine.getRenderImage());

		renderEngine.getRenderImage().transition(render::ImageUsage::STORAGE);

		mPipeline->bind();
		mPipeline->bindDescriptorSet(0, descriptorSet);
		mPipeline->dispatch((windowSize.x + 15) >> 4, (windowSize.y + 15) >> 4);
	}
}
