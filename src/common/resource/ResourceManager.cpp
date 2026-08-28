#include "ResourceManager.h"

#include <cstring>
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
				mResourcePaths.emplace_back(std::string(resourcePath) + "/");
			}
		}
	}

	FindResourceResult ResourceManager::findResource(std::string name, const std::string& extension) const {
		if (!std::regex_match(name, VALID_RESOURCE_NAMES)) {
			throw std::runtime_error("Invalid resource name: " + name);
		}
		name[name.find(':')] = '/';
		for (uint32_t i = 0; i < mResourcePaths.size(); i++) {
			std::string file = mResourcePaths[i] + name + extension;
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
}
