#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace voxel_game::client::render::opengl::opengl_util {
	enum class PushConstantDataType {
		FLOAT,
		DOUBLE,
		INT,
		UINT
	};

	struct PushConstantType {
		uint32_t rows;
		uint32_t columns;
		PushConstantDataType type;
	};

	struct PushConstant {
		std::string name;
		int location;
		uint32_t offset;
		PushConstantType type;
	};

	std::string convertShader(const std::vector<uint32_t>& shaderData);

	std::vector<PushConstant> getPushConstants(size_t shaderCount, const std::vector<uint32_t>* shaderData, unsigned int program);

	void setPushConstantData(const std::vector<PushConstant>& pushConstants, void* data);
}
