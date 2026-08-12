#include "VoxelGameClient.h"

#include "render/RenderEngine.h"
#include "render/vulkan/VulkanEngine.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry &registry) {
		registry.addResource<window::Window, window::glfw::GLFWWindow>(window::glfw::GLFWWindow("Voxel Game", true, 0, 0));
		registry.addResource<render::RenderEngine, render::vulkan::VulkanEngine>(render::vulkan::VulkanEngine(registry));
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();
		while (!window.shouldClose()) {
			window.pollEvents();
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		registry.removeResource<render::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
