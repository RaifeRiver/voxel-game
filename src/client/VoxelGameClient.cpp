/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "VoxelGameClient.h"

#include <numeric>

#include "common/chunk/ChunkData.h"
#include "tracy/Tracy.hpp"

#include "common/component/Transform.h"
#include "common/player/CameraRotation.h"
#include "common/player/Player.h"
#include "common/universe/UniverseLoaderInfo.h"
#include "common/util/Log.h"
#include "player/PlayerInputController.h"
#include "render/ChunkMesh.h"
#include "render/ChunkRenderer.h"
#include "render/SkyRenderer.h"
#include "render/engine/RenderEngine.h"
#include "render/engine/opengl/OpenGLEngine.h"
#include "render/engine/vulkan/VulkanEngine.h"
#include "window/Window.h"
#include "window/glfw/GLFWWindow.h"

namespace voxel_game::client {
	void load(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init client");

		const LaunchOptions& launchOptions = registry.getResource<LaunchOptions>();

		switch (launchOptions.getRenderBackend()) {
			case RenderBackend::OPENGL:
				registry.createResource<window::Window, window::glfw::GLFWWindow>(registry, "Voxel Game", true, 0, 0, true);
				registry.createResource<render::engine::RenderEngine, render::engine::opengl::OpenGLEngine>(registry);
				break;
			case RenderBackend::VULKAN:
				registry.createResource<window::Window, window::glfw::GLFWWindow>(registry, "Voxel Game", true, 0, 0);
				registry.createResource<render::engine::RenderEngine, render::engine::vulkan::VulkanEngine>(registry);
				break;
			default:
				LOG_FATAL("Unsupported render library");
				throw std::runtime_error("Unsupported render library");
		}

		registry.getSystemManager().createSystem<render::SkyRenderer>(registry);
		registry.getSystemManager().createSystem<render::ChunkRenderer>(registry);

		const ecs::Entity player = registry.createEntity();
		voxel_game::player::attachPlayerComponents(registry, player, true);
		registry.getComponent<component::Transform>(player).pos.local = {0, 50, 0};
		auto& cameraRotation = registry.getComponent<voxel_game::player::CameraRotation>(player);
		cameraRotation.pitch = -0.6f;
		registry.getSystemManager().createSystem<player::PlayerInputController>();
		registry.getComponent<universe::UniverseLoaderInfo>(player).radius = launchOptions.getLoadDistance();

		const ecs::Entity planet = registry.createEntity();
		registry.attachComponent<chunk::ChunkData>(planet);
		registry.attachComponent<render::ChunkMeshData>(planet);

		auto& window = registry.getResource<window::Window>();
		window.setLockMouse(true);
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

			registry.update(deltaTime);

			FrameMark;
		}
	}

	void destroy(ecs::ECSRegistry &registry) {
		ZoneScopedN("Destroy client");

		registry.getResource<render::engine::RenderEngine>().waitForGPU();

		for (const ecs::Entity& entity: registry.getEntitiesWithComponents<chunk::ChunkData>()) {
			registry.destroyEntity(entity);
		}

		registry.getSystemManager().removeSystem<render::ChunkRenderer>();
		registry.getSystemManager().removeSystem<render::SkyRenderer>();

		registry.removeResource<render::engine::RenderEngine>();
		registry.removeResource<window::Window>();
	}
}
