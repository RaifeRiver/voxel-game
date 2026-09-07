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

#include "ResourceManager.h"

#include <cstring>
#include <iostream>
#include <regex>

namespace voxel_game::resource {
	const auto VALID_RESOURCE_NAMES = std::regex("[a-z_][a-z_0-9]*:[a-z_][a-z_0-9]*(/[a-z_][a-z_0-9]*)*");

	IResourceLoader::IResourceLoader(const std::vector<std::string>& extensions) : mExtensions(extensions) {}

	ResourceManager::ResourceManager() {
		if (std::filesystem::is_directory("res")) {
			mResourcePaths.emplace_back("res/");
		}
		else if (std::filesystem::is_directory("../res")) {
			mResourcePaths.emplace_back("../res/");
		}
		const char* resourcePath = getenv("VG_RESOURCE_PATH");
		if (resourcePath && std::filesystem::is_directory(resourcePath)) {
			if (resourcePath[strlen(resourcePath) - 1] == '/') {
				mResourcePaths.emplace_back(resourcePath);
			}
			else {
				mResourcePaths.push_back(std::string(resourcePath) + "/");
			}
		}
	}

	FindResourceResult ResourceManager::findResource(std::string name, const std::string& extension, const ResourceType type) const {
		if (!std::regex_match(name, VALID_RESOURCE_NAMES)) {
			throw std::runtime_error("Invalid resource name: " + name);
		}
		name[name.find(':')] = '/';
		const std::string path = to_string(type) + '/' + name + extension;
		for (const std::string& resourcePath : mResourcePaths) {
			std::string file = resourcePath + path;
			if (std::filesystem::is_regular_file(file)) {
				return {
					.found = true,
					.path = file
				};
			}
		}
		return {
			.found = false
		};
	}

	std::vector<FoundResource> ResourceManager::findResources(std::string folder, const std::string& extension, const ResourceType type) const {
		std::vector<FoundResource> resources;
		folder = '/' + folder;
		const std::string typeStr = to_string(type);
		for (const std::string& resourcePath : mResourcePaths) {
			for (const std::filesystem::directory_entry& entry: std::filesystem::directory_iterator(resourcePath + typeStr)) {
				if (std::filesystem::is_directory(entry.status())) {
					std::string folderPath = entry.path().string() + folder;
					for (const std::filesystem::directory_entry& entry2: std::filesystem::recursive_directory_iterator(folderPath)) {
						if (std::filesystem::is_regular_file(entry2.status()) && entry2.path().string().ends_with(extension)) {
							resources.push_back({
								.namespace_ = entry.path().stem(),
								.name = entry2.path().stem(),
								.path = entry2.path().string()
							});
						}
					}
				}
			}
		}
		return resources;
	}
}
