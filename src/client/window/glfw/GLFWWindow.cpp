#include "GLFWWindow.h"

#include <stdexcept>

namespace voxel_game::client::window::glfw {
	GLFWWindow::GLFWWindow(const std::string& name, const bool fullscreen, const int width, const int height) {
		init();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

		if (fullscreen) {
			mWindow = glfwCreateWindow(videoMode->width, videoMode->height, name.c_str(), monitor, nullptr);
		}
		else {
			mWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		}
		if (!mWindow) {
			throw std::runtime_error("Failed to create GLFW window");
		}
	}

	std::vector<const char*> GLFWWindow::getRequiredVulkanExtensions() {
		uint32_t count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&count);
		return {extensions, extensions + count};
	}

	// ReSharper disable CppParameterMayBeConst
	bool GLFWWindow::getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, const uint32_t queueFamily) {
		return glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, queueFamily);
	}

	VkResult GLFWWindow::createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) {
		return glfwCreateWindowSurface(instance, mWindow, nullptr, surface);
	}

	glm::uvec2 GLFWWindow::getSize() {
		int width;
		int height;
		glfwGetWindowSize(mWindow, &width, &height);
		return {width, height};
	}

	bool GLFWWindow::shouldClose() {
		return glfwWindowShouldClose(mWindow);
	}

	void GLFWWindow::pollEvents() {
		glfwPollEvents();
	}

	void GLFWWindow::setVisible(bool visible) {
		if (visible) {
			glfwShowWindow(mWindow);
		}
		else {
			glfwHideWindow(mWindow);
		}
	}

	void GLFWWindow::destroy() {
		glfwDestroyWindow(mWindow);
	}

	// ReSharper restore CppParameterMayBeConst

	void GLFWWindow::init() {
		static bool init = false;
		if (init) {
			return;
		}

		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialise GLFW");
		}
		init = true;
	}
}
