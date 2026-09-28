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

#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

#include "glm/vec2.hpp"

#include "common/ecs/Resource.h"

namespace voxel_game::client::window {
	using WindowResizeCallback = std::function<void(glm::uvec2)>;

	enum class Key {
		KEY_A,
		KEY_D,
		KEY_S,
		KEY_W,

		KEY_LEFT_SHIFT,
		KEY_SPACE
	};

	class Window : public ecs::Resource<Window> {
	public:
		virtual void swapOpenGLBuffers() = 0;

		virtual std::vector<const char*> getRequiredVulkanExtensions() = 0;

		virtual bool getVulkanPhysicalDevicePresentationSupport(VkInstance instance, VkPhysicalDevice physicalDevice, uint32_t queueFamily) = 0;

		virtual VkResult createVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) = 0;

		virtual glm::uvec2 getSize() = 0;

		virtual bool shouldClose() = 0;

		virtual void pollEvents() = 0;

		virtual void setVisible(bool visible) = 0;

		virtual void setResizeCallback(const WindowResizeCallback& callback) = 0;

		virtual bool isKeyPressed(Key key) = 0;

		virtual glm::vec2 getMouseMovement() = 0;

		virtual void setLockMouse(bool lockMouse) = 0;
	};
}
