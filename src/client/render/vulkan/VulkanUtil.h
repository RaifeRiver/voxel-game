#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

namespace voxel_game::client::render::vulkan::vulkan_util {
	inline bool vkCheck(const VkResult result, const bool fatal = true) {
		if (result != VK_SUCCESS) {
			if (fatal) {
				throw std::runtime_error("VK error: " + std::string(string_VkResult(result)));
			}
			std::cerr << "VK error: " << result << std::endl;
			return false;
		}
		return true;
	}

	inline VkApplicationInfo applicationInfo(const std::string &applicationName, const uint32_t apiVersion) {
		return {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = applicationName.c_str(),
			.apiVersion = apiVersion,
		};
	}

	inline VkInstanceCreateInfo instanceCreateInfo(const VkApplicationInfo& applicationInfo, const std::vector<const char*>& extensionNames) {
		return {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo,
			.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
			.ppEnabledExtensionNames = extensionNames.data()
		};
	}
}