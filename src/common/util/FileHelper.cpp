#include "FileHelper.h"

namespace voxel_game::util {
	std::string readFileAsString(const std::string& path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + path);
		}
		const std::ifstream::pos_type fileSize = file.tellg();
		std::string buffer;
		buffer.resize(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
		return buffer;
	}
}
