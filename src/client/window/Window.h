#pragma once

#include <functional>
#include <vector>
#include <vulkan/vulkan.h>

#include "glm/vec2.hpp"

#include "common/ecs/Resource.h"

namespace voxel_game::client::window {
	using WindowResizeCallback = std::function<void(glm::uvec2)>;

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
	};
}
