#pragma once

#include <memory>

#include "WindowManager.h"

namespace voxel_game::client::renderer {
	class RenderEngine {
	public:
		RenderEngine();

		[[nodiscard]] WindowManager * getWindowManager() const {
			return mWindowManager.get();
		}

		void render();

		void destroy() const;

	private:
		std::unique_ptr<WindowManager> mWindowManager = nullptr;
	};
}
