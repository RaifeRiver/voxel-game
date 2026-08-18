#pragma once

#include "volk.h"

#include "client/render/Shader.h"

namespace voxel_game::client::render::vulkan {
	VkShaderStageFlags toVKShaderStage(ShaderStage shaderStage);
}
