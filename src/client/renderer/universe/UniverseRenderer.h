#pragma once

#include <vulkan/vulkan.h>

#include "client/renderer/RenderContext.h"

namespace voxel_game::client::renderer::universe {
	struct GradientPushConstants {
		float r;
		float g;
		float b;
	};

	class UniverseRenderer {
	public:
		void init(RenderContext *renderContext);

		void render(RenderContext *renderContext, VkCommandBuffer commandBuffer) const;

		void destroy(const RenderContext *renderContext) const;

	private:
		VkPipeline mGradientPipeline = nullptr;
		VkPipelineLayout mGradientPipelineLayout = nullptr;

		void initPipelines(RenderContext *renderContext);
	};
}
