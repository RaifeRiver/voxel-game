#pragma once

#include <string>

// ReSharper disable CppUnusedIncludeDirective
#include "glad/glad.h"
#include "volk.h"
// ReSharper restore CppUnusedIncludeDirective
#include "GLFW/glfw3.h"

#include "client/window/Window.h"

namespace voxel_game::client::window::glfw {
	int toGLFWKey(Key key);

	class GLFWWindow : public Window {
	public:
		GLFWWindow(const std::string& name, bool fullscreen, int width, int height, bool context = false);

		void swapOpenGLBuffers() override;

		std::vector<const char*> getRequiredVulkanExtensions() override;

		bool getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamily) override;

		VkResult createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) override;

		glm::uvec2 getSize() override;

		bool shouldClose() override;

		void pollEvents() override;

		void setVisible(bool visible) override;

		void setResizeCallback(const WindowResizeCallback& callback) override;

		bool isKeyPressed(Key key) override;

		glm::vec2 getMouseMovement() override;

		void setLockMouse(bool lockMouse) override;

		void destroy() override;

	private:
		GLFWwindow* mWindow;
		WindowResizeCallback mResizeCallback;
		glm::vec2 mMouseMovement;

		static void init();
	};
}
