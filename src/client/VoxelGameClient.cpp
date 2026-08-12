#include "VoxelGameClient.h"

#include "render/RenderEngine.h"
#include "render/vulkan/VulkanEngine.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry) {
		registry.addResource<window::Window, window::glfw::GLFWWindow>(window::glfw::GLFWWindow("Voxel Game", true, 0, 0));
		registry.addResource<render::RenderEngine, render::vulkan::VulkanEngine>(render::vulkan::VulkanEngine(registry));
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();

		const std::chrono::time_point<std::chrono::system_clock> lastTime = std::chrono::high_resolution_clock::now();
		while (!window.shouldClose()) {
			std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

			window.pollEvents();

			registry.getCommandQueue().execute(registry);
			registry.getSystemManager().runSystems(registry, deltaTime);
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		registry.removeResource<render::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
