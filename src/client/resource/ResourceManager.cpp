#include "ResourceManager.h"

namespace voxel_game::client::resource {
	void ResourceManager::unload(const std::string &name) {
		auto it = mResources.find(name);
		if (it == mResources.end()) {
			throw std::runtime_error("Resource not loaded");
		}
		it->second->destroy();
		mResources.erase(it);
	}

	void ResourceManager::destroy() {
		for (std::unique_ptr<Resource> &resource: mResources | std::views::values) {
			resource->destroy();
		}
		mResources.clear();
	}
}
