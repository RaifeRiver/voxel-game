#include "VoxelGameClient.h"

#include "common/player/Player.h"
#include "common/util/Log.h"
#include "render/engine/RenderEngine.h"
#include "render/engine/opengl/OpenGLEngine.h"
#include "render/engine/vulkan/VulkanEngine.h"
#include "render/SkyRenderer.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry, const CommandLineArguments& args) {
		switch (args.getRenderLibrary()) {
			case RenderLibrary::OPENGL:
				registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0, true);
				registry.createResource<render::RenderEngine, render::opengl::OpenGLEngine>(registry);
				break;
			case RenderLibrary::VULKAN:
				registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0);
				registry.createResource<render::RenderEngine, render::vulkan::VulkanEngine>(registry);
				break;
			default:
				LOG_FATAL("Unsupported render library");
				throw std::runtime_error("Unsupported render library");
		}

		registry.getSystemManager().createSystem<render::SkyRenderer>(registry);

		player::attachPlayerComponents(registry, registry.createEntity(), true);
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

		registry.getSystemManager().removeSystem<render::SkyRenderer>();

		registry.removeResource<render::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
