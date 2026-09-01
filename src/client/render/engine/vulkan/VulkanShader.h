#pragma once

#include "volk.h"

#include "client/render/engine/Shader.h"

namespace voxel_game::client::render::vulkan {
	VkShaderStageFlags toVKShaderStage(ShaderStage shaderStage);
}
