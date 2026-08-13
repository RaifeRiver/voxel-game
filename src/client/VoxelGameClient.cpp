#include "VoxelGameClient.h"

#include <cmath>

#include "render/RenderEngine.h"
#include "render/opengl/OpenGLEngine.h"
#include "render/vulkan/VulkanEngine.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	constexpr bool USE_OPENGL = true;

	void load(ecs::ECSRegistry& registry) {
		if (USE_OPENGL) {
			registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0, true);
			registry.createResource<render::RenderEngine, render::opengl::OpenGLEngine>(registry);
		}
		else {
			registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0);
			registry.createResource<render::RenderEngine, render::vulkan::VulkanEngine>(registry);
		}

		registry.getSystemManager().registerSystem(ecs::Stage::BACKGROUND_RENDER, [](ecs::ECSRegistry& r, const float dt) {
			static float mTime = 0.0f;
			mTime += dt;
			r.getResource<render::RenderEngine>().getRenderImage().clearColour({std::abs(std::sin(mTime)), 0.0f, 0.0f, 1.0f});
		});
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();

		std::chrono::time_point<std::chrono::system_clock> lastTime = std::chrono::high_resolution_clock::now();
		while (!window.shouldClose()) {
			std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::high_resolution_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

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
