#include "BackgroundRenderSystem.h"

#include "client/render/RenderEngine.h"
#include "client/render/Shader.h"
#include "client/window/Window.h"
#include "common/resource/ResourceManager.h"

namespace voxel_game::client::system {
	BackgroundRenderSystem::BackgroundRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		mPipeline = renderEngine.createComputePipeline(resourceManager.findResource("voxel_game:shaders/gradient", ".comp.spv", resource::ResourceType::ASSET).path);

		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->addBinding(0, render::DescriptorType::IMAGE)->build(render::FRAME_OVERLAP, render::ShaderStage::COMPUTE);

		for (std::unique_ptr<render::DescriptorSet>& descriptorSet : mDescriptorSets) {
			descriptorSet = mDescriptorAllocator->allocate();
		}
	}

	void BackgroundRenderSystem::backgroundRender(ecs::ECSRegistry& registry, const float deltaTime) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		auto& window = registry.getResource<window::Window>();

		const glm::uvec2 windowSize = window.getSize();

		mTime += deltaTime;

		render::DescriptorSet* descriptorSet = mDescriptorSets[renderEngine.getFrame() % render::FRAME_OVERLAP].get();
		descriptorSet->setBinding(0, &renderEngine.getRenderImage());

		renderEngine.getRenderImage().transition(render::ImageUsage::STORAGE);

		mPipeline->bind();
		mPipeline->bindDescriptorSet(0, descriptorSet);
		mPipeline->setPushConstants(&mTime);
		mPipeline->dispatch((windowSize.x + 15) >> 4, (windowSize.y + 15) >> 4);
	}
}
