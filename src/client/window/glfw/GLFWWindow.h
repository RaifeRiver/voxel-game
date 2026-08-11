#pragma once

#include <string>
#include <vulkan/vulkan.h>

#include "GLFW/glfw3.h"

#include "client/window/Window.h"

namespace voxel_game::client::window::glfw {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(const std::string& name, bool fullscreen, int width, int height, bool context);

		std::vector<const char*> getRequiredVulkanExtensions() override;

		bool getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamily) override;

	private:
		GLFWwindow* mWindow;

		static void init();
	};
}
