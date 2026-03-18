#include "WindowManager.h"

#include <stdexcept>

namespace voxel_game::client::renderer {
	WindowManager::WindowManager() {
		if (sWindowCount++ == 0) {
			glfwInit();
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		mWindow = glfwCreateWindow(1280, 720, "Voxel Game", nullptr, nullptr);
		if (!mWindow) {
			sWindowCount--;
			if (sWindowCount == 0) {
				glfwTerminate();
			}
			throw std::runtime_error("Failed to create GLFW window");
		}
	}

	void WindowManager::showWindow() const {
		glfwShowWindow(mWindow);
	}

	void WindowManager::hideWindow() const {
		glfwHideWindow(mWindow);
	}

	bool WindowManager::shouldClose() const {
		return glfwWindowShouldClose(mWindow);
	}

	void WindowManager::pollEvents() {
		glfwPollEvents();
	}

	void WindowManager::destroy() const {
		glfwDestroyWindow(mWindow);
		if (--sWindowCount == 0) {
			glfwTerminate();
		}
	}
}
