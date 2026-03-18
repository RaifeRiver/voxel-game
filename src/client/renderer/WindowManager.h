#pragma once

#include "GLFW/glfw3.h"

namespace voxel_game::client::renderer {
	class WindowManager {
	public:
		WindowManager();

		void showWindow() const;

		void hideWindow() const;

		[[nodiscard]] bool shouldClose() const;

		static void pollEvents();

		void destroy() const;

	private:
		static inline int sWindowCount = 0;

		GLFWwindow * mWindow;
	};
}
