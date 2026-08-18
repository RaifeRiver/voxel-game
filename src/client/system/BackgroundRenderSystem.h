#pragma once

#include "client/render/DescriptorAllocator.h"
#include "client/render/vulkan/VulkanEngine.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::system {
	class BackgroundRenderSystem : public ecs::System {
	public:
		explicit BackgroundRenderSystem(ecs::ECSRegistry& registry);

		void backgroundRender(ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<render::DescriptorAllocator> mDescriptorAllocator = nullptr;
		std::unique_ptr<render::DescriptorSet> mDescriptorSets[render::vulkan::FRAME_OVERLAP];
	};
}
