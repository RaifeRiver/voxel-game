#pragma once

#include <memory>

#include "RenderContext.h"
#include "WindowManager.h"

namespace voxel_game::client::renderer {
	class RenderEngine {
	public:
		RenderEngine();

		[[nodiscard]] WindowManager * getWindowManager() const {
			return mWindowManager.get();
		}

		[[nodiscard]] RenderContext * getRenderContext() const {
			return mRenderContext.get();
		}

		void render();

		void destroy() const;

	private:
		std::unique_ptr<WindowManager> mWindowManager = nullptr;
		std::unique_ptr<RenderContext> mRenderContext = nullptr;
		bool needsResize = false;

		void renderBackground(VkCommandBuffer commandBuffer) const;
	};
}
