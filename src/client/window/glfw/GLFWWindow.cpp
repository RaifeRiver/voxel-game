#include "GLFWWindow.h"

#include <stdexcept>

namespace voxel_game::client::window::glfw {
	GLFWWindow::GLFWWindow(const std::string& name, const bool fullscreen, const int width, const int height, const bool context) {
		init();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

		glfwDefaultWindowHints();
		if (context) {
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
		else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);

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
