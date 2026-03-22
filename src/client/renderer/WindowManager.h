#pragma once

#include <vulkan/vulkan.h>

#include "GLFW/glfw3.h"

namespace voxel_game::client::renderer {
	class WindowManager {
	public:
		WindowManager();

		void showWindow() const;

		void hideWindow() const;

		[[nodiscard]] bool shouldClose() const;

		static void pollEvents();

		void createSurface(VkInstance instance, VkSurfaceKHR* surface) const;

		[[nodiscard]] uint32_t getWidth() const;

		[[nodiscard]] uint32_t getHeight() const;

		[[nodiscard]] VkExtent2D getSize() const;

		void destroy() const;

	private:
		static inline int sWindowCount = 0;

		GLFWwindow * mWindow;
	};
}
