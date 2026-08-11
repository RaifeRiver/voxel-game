#include "VoxelGameClient.h"

#include "render/RenderContext.h"
#include "render/vulkan/VulkanContext.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry &registry) {
		registry.addResource<window::Window, window::glfw::GLFWWindow>(window::glfw::GLFWWindow("Voxel Game", true, 0, 0));
		registry.addResource<render::RenderContext, render::vulkan::VulkanContext>(render::vulkan::VulkanContext(registry));
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();
		while (!window.shouldClose()) {
			window.pollEvents();
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		registry.removeResource<render::RenderContext>();
		registry.removeResource<window::Window>();
	}
}
