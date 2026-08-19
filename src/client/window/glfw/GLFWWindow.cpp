#include "GLFWWindow.h"

#include <iostream>
#include <stdexcept>

namespace voxel_game::client::window::glfw {
	GLFWWindow::GLFWWindow(const std::string& name, const bool fullscreen, const int width, const int height, const bool context) {
		init();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

		glfwDefaultWindowHints();
		if (context) {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef VG_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif
		}
		else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
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

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int w, int h) {
			const GLFWWindow* window2 = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
			if (window2->mResizeCallback) {
				window2->mResizeCallback({w, h});
			}
		});

		if (context) {
			glfwMakeContextCurrent(mWindow);
		}
	}

	void GLFWWindow::swapOpenGLBuffers() {
		glfwSwapBuffers(mWindow);
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
	// ReSharper restore CppParameterMayBeConst

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

	void GLFWWindow::setVisible(const bool visible) {
		if (visible) {
			glfwShowWindow(mWindow);
		}
		else {
			glfwHideWindow(mWindow);
		}
	}

	void GLFWWindow::setResizeCallback(const WindowResizeCallback& callback) {
		mResizeCallback = callback;
	}

	void GLFWWindow::destroy() {
		glfwDestroyWindow(mWindow);
	}

	void GLFWWindow::init() {
		static bool init = false;
		if (init) {
			return;
		}

		glfwSetErrorCallback([](const int errorCode, const char* description) {
			std::cerr << "GLFW Error " << errorCode << ": " << description << std::endl;
		});

		if (!glfwInit()) {
			throw std::runtime_error("Failed to initialise GLFW");
		}
		init = true;
	}
}
