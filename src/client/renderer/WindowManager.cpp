#include "WindowManager.h"

#include <stdexcept>

namespace voxel_game::client::renderer {
	WindowManager::WindowManager() {
		if (sWindowCount++ == 0) {
			glfwInit();
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		mWindow = glfwCreateWindow(1280, 720, "Voxel Game", nullptr, nullptr);
		if (!mWindow) {
			sWindowCount--;
			if (sWindowCount == 0) {
				glfwTerminate();
			}
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(mWindow, this);

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, const int key, int, const int action, int) {
			if (action == GLFW_PRESS) {
				if (key == GLFW_KEY_F11) {
					if (glfwGetWindowMonitor(window) != nullptr) {
						glfwSetWindowMonitor(window, nullptr, 0, 0, 1280, 720, GLFW_DONT_CARE);
					}
					else {
						GLFWmonitor* monitor = glfwGetPrimaryMonitor();
						const GLFWvidmode* mode = glfwGetVideoMode(monitor);
						glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
					}
				}
			}
		});
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

	void WindowManager::createSurface(const VkInstance instance, VkSurfaceKHR* surface) const {
		glfwCreateWindowSurface(instance, mWindow, nullptr, surface);
	}

	uint32_t WindowManager::getWidth() const {
		int width;
		glfwGetWindowSize(mWindow, &width, nullptr);
		return width;
	}

	uint32_t WindowManager::getHeight() const {
		int height;
		glfwGetWindowSize(mWindow, nullptr, &height);
		return height;
	}

	VkExtent2D WindowManager::getSize() const {
		int width;
		int height;
		glfwGetWindowSize(mWindow, &width, &height);
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	void WindowManager::destroy() const {
		glfwDestroyWindow(mWindow);
		if (--sWindowCount == 0) {
			glfwTerminate();
		}
	}
}
