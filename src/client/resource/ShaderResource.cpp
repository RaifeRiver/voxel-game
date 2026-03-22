#include "ShaderResource.h"

#include <cstdint>
#include <fstream>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "client/VoxelGameClient.h"
#include "client/renderer/VulkanUtil.h"

namespace voxel_game::client::resource {
	ShaderResource::ShaderResource(const std::string &name): Resource(name) {}

	void ShaderResource::init(const std::string path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open shader file");
		}
		size_t fileSize = file.tellg();
		std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));
		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
		file.close();

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = fileSize;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

		VK_CHECK(vkCreateShaderModule(VoxelGameClient::getClient()->getRenderEngine()->getRenderContext()->getDevice(), &createInfo, nullptr, &mShaderModule));
	}

	void ShaderResource::destroy() {
		vkDestroyShaderModule(VoxelGameClient::getClient()->getRenderEngine()->getRenderContext()->getDevice(), mShaderModule, nullptr);
	}
}
