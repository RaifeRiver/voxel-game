#pragma once

#include <memory>
#include <ranges>
#include <unordered_map>

#include "Resource.h"

namespace voxel_game::client::resource {
	class ResourceManager {
	public:
		template<std::derived_from<Resource> T>	T* load(const std::string &name, const std::string &path) {
			auto it = mResources.find(name);
			if (it != mResources.end()) {
				return static_cast<T*>(it->second.get());
			}
			std::unique_ptr<T> resource = std::make_unique<T>(name);
			T* resourcePointer = resource.get();
			resource->T::init("../resources/" + path);
			mResources[name] = std::move(resource);
			return resourcePointer;
		}

		template<std::derived_from<Resource> T>	T* get(const std::string &name) {
			auto it = mResources.find(name);
			return it == mResources.end()? nullptr: static_cast<T*>(it->second.get());
		}

		void unload(const std::string &name);

		void destroy();

	private:
		std::unordered_map<std::string, std::unique_ptr<Resource>> mResources = {};
	};
}
