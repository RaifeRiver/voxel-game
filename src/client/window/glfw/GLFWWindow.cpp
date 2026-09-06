#include "GLFWWindow.h"

#include <iostream>
#include <stdexcept>

#include "client/render/engine/RenderEngine.h"
#include "common/util/Log.h"
#include "tracy/Tracy.hpp"

namespace voxel_game::client::window::glfw {
	int toGLFWKey(const Key key) {
		switch (key) {
			case Key::KEY_A:
				return GLFW_KEY_A;
			case Key::KEY_D:
				return GLFW_KEY_D;
			case Key::KEY_S:
				return GLFW_KEY_S;
			case Key::KEY_W:
				return GLFW_KEY_W;
			case Key::KEY_LEFT_SHIFT:
				return GLFW_KEY_LEFT_SHIFT;
			case Key::KEY_SPACE:
				return GLFW_KEY_SPACE;
			default:
				throw std::runtime_error("Unsupported key");
		}
	}

	GLFWWindow::GLFWWindow(const std::string& name, const bool fullscreen, const int width, const int height, const bool context) {
		ZoneScopedN("Create GLFW window");

		LOG_INFO("Using GLFW window");

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
		if (render::engine::ENABLE_VSYNC) {
			glfwWindowHint(GLFW_REFRESH_RATE, videoMode->refreshRate);
		}
		glfwWindowHint(GLFW_RED_BITS, videoMode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, videoMode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, videoMode->blueBits);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHintString(GLFW_WAYLAND_APP_ID, "Voxel Game");

		if (fullscreen) {
			mWindow = glfwCreateWindow(videoMode->width, videoMode->height, name.c_str(), monitor, nullptr);
		}
		else {
			mWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
		}
		if (!mWindow) {
			LOG_FATAL("Failed to create GLFW window");
			throw std::runtime_error("Failed to create GLFW window");
		}

		glfwSetWindowUserPointer(mWindow, this);
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int w, int h) {
			const GLFWWindow* window2 = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
			if (window2->mResizeCallback) {
				window2->mResizeCallback({w, h});
			}
		});
		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, const double x, const double y) {
			static glm::dvec2 previous = {};
			const glm::dvec2 current = {x, y};
			const glm::dvec2 movement = current - previous;
			previous = current;

			const auto window2 = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
			window2->mMouseMovement += movement;
		});

		if (context) {
			glfwMakeContextCurrent(mWindow);
			if (!render::engine::ENABLE_VSYNC) {
				glfwSwapInterval(0);
			}
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
		glfwGetFramebufferSize(mWindow, &width, &height);
		return {width, height};
	}

	bool GLFWWindow::shouldClose() {
		return glfwWindowShouldClose(mWindow);
	}

	void GLFWWindow::pollEvents() {
		mMouseMovement = {};
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

	bool GLFWWindow::isKeyPressed(const Key key) {
		return glfwGetKey(mWindow, toGLFWKey(key)) == GLFW_PRESS;
	}

	glm::vec2 GLFWWindow::getMouseMovement() {
		return mMouseMovement;
	}

	void GLFWWindow::setLockMouse(const bool lockMouse) {
		if (lockMouse) {
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void GLFWWindow::destroy() {
		glfwDestroyWindow(mWindow);
	}

	void GLFWWindow::init() {
		static bool init = false;
		if (init) {
			return;
		}

		ZoneScopedN("Init GLFW");

		glfwSetErrorCallback([](const int errorCode, const char* description) {
			LOG_ERROR("GLFW Error {}: {}", errorCode, description);
		});

		if (!glfwInit()) {
			LOG_FATAL("Failed to initialize GLFW");
			throw std::runtime_error("Failed to initialise GLFW");
		}
		init = true;
	}
}
