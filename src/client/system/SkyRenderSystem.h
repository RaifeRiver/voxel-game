#pragma once

#include "client/render/DescriptorAllocator.h"
#include "client/render/RenderEngine.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::system {
	class SkyRenderSystem : public ecs::System {
	public:
		explicit SkyRenderSystem(ecs::ECSRegistry& registry);

		void backgroundRender(ecs::ECSRegistry& registry, float deltaTime) override;

	private:
		std::unique_ptr<render::RenderPipeline> mPipeline = nullptr;
	};
}
