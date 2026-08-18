#include "BackgroundRenderSystem.h"

#include "client/render/RenderEngine.h"
#include "client/render/Shader.h"

namespace voxel_game::client::system {
	BackgroundRenderSystem::BackgroundRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->addBinding(0, render::DescriptorType::IMAGE)->build(render::vulkan::FRAME_OVERLAP, render::ShaderStage::COMPUTE);
		for (std::unique_ptr<render::DescriptorSet>& descriptorSet : mDescriptorSets) {
			descriptorSet = mDescriptorAllocator->allocate();
		}
	}

	void BackgroundRenderSystem::backgroundRender(ecs::ECSRegistry& registry, float) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		renderEngine.getRenderImage().clearColour({0.1, 0, 0.5, 1});
	}
}
