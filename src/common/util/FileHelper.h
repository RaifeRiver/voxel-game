#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace voxel_game::util {
	template <typename T> [[nodiscard]] std::vector<T> readFile(const std::string& path) {
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + path);
		}
		const std::ifstream::pos_type fileSize = file.tellg();
		std::vector<T> buffer(fileSize / sizeof(T));
		file.seekg(0);
		file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
		file.close();
		return buffer;
	}

	[[nodiscard]] std::string readFileAsString(const std::string& path);
}
