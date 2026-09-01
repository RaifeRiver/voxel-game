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
