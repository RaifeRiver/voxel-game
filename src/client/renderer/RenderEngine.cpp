#include "RenderEngine.h"

#include <memory>

namespace voxel_game::client::renderer {
	RenderEngine::RenderEngine() {
		mWindowManager = std::make_unique<WindowManager>();
		mWindowManager->showWindow();
	}

	void RenderEngine::render() {
		WindowManager::pollEvents();
	}

	void RenderEngine::destroy() const {
		mWindowManager->destroy();
	}
}
