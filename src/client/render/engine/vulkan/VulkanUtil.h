#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "VulkanEngine.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::vulkan::vulkan_util {
	std::string vkResultString(VkResult result);

	inline bool vkCheck(const VkResult result, const bool fatal = true) {
		if (result != VK_SUCCESS) {
				std::string resultString = vkResultString(result);
			if (fatal) {
				LOG_FATAL("VK fatal error: {}", resultString);
				throw std::runtime_error("VK error: " + resultString);
			}
			LOG_ERROR("VK error: {}", resultString);
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

	inline VkInstanceCreateInfo instanceCreateInfo(const VkApplicationInfo& applicationInfo, const std::vector<const char*>& extensionNames, const std::vector<const char*>& layerNames) {
		return {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = static_cast<uint32_t>(layerNames.size()),
			.ppEnabledLayerNames = layerNames.data(),
			.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
			.ppEnabledExtensionNames = extensionNames.data()
		};
	}

	inline VkDeviceQueueCreateInfo deviceQueueCreateInfo(const uint32_t queueFamilyIndex, const float* queuePriorities) {
		return {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamilyIndex,
			.queueCount = 1,
			.pQueuePriorities = queuePriorities
		};
	}

	inline VkDeviceCreateInfo deviceCreateInfo(const uint32_t queueCreateInfoCount, VkDeviceQueueCreateInfo* queueCreateInfos, const std::vector<const char*>& extensions, VkPhysicalDeviceFeatures* features, const void* next = nullptr) {
		return {
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = next,
			.queueCreateInfoCount = queueCreateInfoCount,
			.pQueueCreateInfos = queueCreateInfos,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
			.pEnabledFeatures = features
		};
	}

	std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts(const VulkanEngine* vulkanEngine, size_t shaderCount, const std::vector<uint32_t>* shaderData);

	std::vector<VkPushConstantRange> getPushConstantRanges(size_t shaderCount, const std::vector<uint32_t>* shaderData);
}
