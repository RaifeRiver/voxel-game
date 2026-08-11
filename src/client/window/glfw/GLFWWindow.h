#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include "GLFW/glfw3.h"

#include "client/window/Window.h"

namespace voxel_game::client::window::glfw {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const std::string& name, bool fullscreen, int width, int height);

		std::vector<const char*> getRequiredVulkanExtensions() override;

		bool getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamily) override;

		VkResult createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) override;

		glm::uvec2 getSize() override;

		bool shouldClose() override;

		void pollEvents() override;

		void setVisible(bool visible) override;

		void destroy() override;

	private:
		GLFWwindow* mWindow;

		static void init();
	};
}
