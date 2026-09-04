#include "VoxelGameClient.h"

#include <numeric>

#include "common/player/Player.h"
#include "common/util/Log.h"
#include "render/ChunkRenderer.h"
#include "render/SkyRenderer.h"
#include "render/engine/RenderEngine.h"
#include "render/engine/opengl/OpenGLEngine.h"
#include "render/engine/vulkan/VulkanEngine.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry, const CommandLineArguments& args) {
		switch (args.getRenderLibrary()) {
			case RenderLibrary::OPENGL:
				registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0, true);
				registry.createResource<render::engine::RenderEngine, render::engine::opengl::OpenGLEngine>(registry);
				break;
			case RenderLibrary::VULKAN:
				registry.createResource<window::Window, window::glfw::GLFWWindow>("Voxel Game", true, 0, 0);
				registry.createResource<render::engine::RenderEngine, render::engine::vulkan::VulkanEngine>(registry);
				break;
			default:
				LOG_FATAL("Unsupported render library");
				throw std::runtime_error("Unsupported render library");
		}

		registry.getSystemManager().createSystem<render::SkyRenderer>(registry);
		registry.getSystemManager().createSystem<render::ChunkRenderer>(registry);

		player::attachPlayerComponents(registry, registry.createEntity(), true);
	}

	void run(ecs::ECSRegistry &registry) {
		auto& window = registry.getResource<window::Window>();

		std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();
		uint64_t frame = 0;
		float frameTimes[1000];
		while (!window.shouldClose()) {
			std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
			const float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
			lastTime = currentTime;

			frameTimes[frame++] = deltaTime;
			if (frame == 1000) {
				frame = 0;
				const float averageFrameTime = std::accumulate(frameTimes, frameTimes + 1000, 0.0f) / 1000.0f;
				std::cout << "\rFPS: " << (1.0f / averageFrameTime) << std::flush;
			}

			window.pollEvents();

			registry.getCommandQueue().execute(registry);
			registry.getSystemManager().runSystems(registry, deltaTime);
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		registry.getResource<render::engine::RenderEngine>().waitForGPU();

		registry.getSystemManager().removeSystem<render::ChunkRenderer>();
		registry.getSystemManager().removeSystem<render::SkyRenderer>();

		registry.removeResource<render::engine::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
