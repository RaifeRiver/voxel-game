#pragma once

#include <format>
#include <stdexcept>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define VK_CHECK(x) do { \
	VkResult result = x; \
	if (result != VK_SUCCESS) { \
		throw std::runtime_error(std::format("Vulkan error: {} at {}:{}", string_VkResult(result), __FILE__, __LINE__)); \
	} \
} \
while (0)
