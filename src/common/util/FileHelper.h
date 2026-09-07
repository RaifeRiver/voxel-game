/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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

	template <typename T> void writeToFile(std::ofstream& file, const T value) {
		file.write(reinterpret_cast<const char*>(&value), sizeof(value));
	}
}
