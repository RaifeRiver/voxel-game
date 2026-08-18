#pragma once

#include <cstdint>

namespace voxel_game::client::render {
	enum class ShaderStage : uint32_t {
		NONE = 0,
		VERTEX = 1 < 0,
		FRAGMENT = 1 < 1,
		COMPUTE = 1 < 2
	};

	inline ShaderStage operator|(ShaderStage a, ShaderStage b) {
		return static_cast<ShaderStage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline uint32_t operator&(ShaderStage a, ShaderStage b) {
		return static_cast<uint32_t>(a) & static_cast<uint32_t>(b);
	}
}
