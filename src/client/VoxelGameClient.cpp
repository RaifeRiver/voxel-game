#include "VoxelGameClient.h"

#include "render/RenderEngine.h"
#include "render/opengl/OpenGLEngine.h"
#include "render/vulkan/VulkanEngine.h"
#include "system/BackgroundRenderSystem.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	constexpr bool USE_OPENGL = false;

	static uint32_t backgroundRenderSystemID;

	void load(ecs::ECSRegistry& registry) {
		if (USE_OPENGL) {
			registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0, true);
			registry.createResource<render::RenderEngine, render::opengl::OpenGLEngine>(registry);
		}
		else {
			registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0);
			registry.createResource<render::RenderEngine, render::vulkan::VulkanEngine>(registry);
		}

		backgroundRenderSystemID = registry.getSystemManager().createSystem<system::BackgroundRenderSystem>(registry);
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();

		std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();
		while (!window.shouldClose()) {
			std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

			window.pollEvents();

			registry.getCommandQueue().execute(registry);
			registry.getSystemManager().runSystems(registry, deltaTime);
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		registry.getResource<render::RenderEngine>().waitForGPU();

		registry.getSystemManager().removeSystem(backgroundRenderSystemID);

		registry.removeResource<render::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
