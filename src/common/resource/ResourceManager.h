#pragma once

#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include "Resource.h"
#include "common/ecs/Resource.h"
#include "common/util/MacroHelper.h"

namespace voxel_game::resource {
	class IResourceLoader {
	public:
		virtual std::unique_ptr<IResource> loadResource(const std::string& path) = 0;

		virtual ~IResourceLoader() = default;
	};

	template <typename T> requires std::derived_from<T, Resource<T>> class ResourceLoader : public IResourceLoader {
	public:
		std::unique_ptr<T> loadResource(const std::string& path) override = 0;
	};

	class ResourceManager : public ecs::Resource<ResourceManager> {
	public:
		ResourceManager() {
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

		template <typename T> requires std::derived_from<T, Resource<T>> void addResourceLoader(ResourceLoader<T>* loader) {
#if VG_SIDE != CLIENT
			if (T::getType() == ResourceType::ASSET) {
				throw std::runtime_error(std::string("Asset loader for type ") + MACRO_TO_STRING(T) + " added on invalid side " + MACRO_TO_STRING(VG_SIDE));
			}
#endif
			const uint32_t id = T::getID();
			if (id >= mResourceLoaders.size()) {
				mResourceLoaders.resize(id + 1);
			}
			mResourceLoaders[id] = std::unique_ptr(loader);
		}

		template <typename T> requires std::derived_from<T, Resource<T>> T& loadResource(const std::string& name) {
#if VG_SIDE != CLIENT
			if (T::getType() == ResourceType::ASSET) {
				throw std::runtime_error(std::string("Asset for type ") + MACRO_TO_STRING(T) + " added on invalid side " + MACRO_TO_STRING(VG_SIDE));
			}
#endif
			const uint32_t id = T::getID();
			if (id >= mResourceLoaders.size() || !mResourceLoaders[id]) {
				throw std::runtime_error(std::string("No resource loader for type ") + MACRO_TO_STRING(T));
			}
			if (id >= mResources.size()) {
				mResources.resize(id + 1);
			}
			mResources[id][name] = std::move(mResourceLoaders[id]->loadResource(name));
			return *mResources[id][name];
		}

		template <typename T> requires std::derived_from<T, Resource<T>> T& getResource(const std::string& name) {
			const uint32_t id = T::getID();
			if (id >= mResources.size() || !mResources[id][name]) {
				throw std::runtime_error("No resource loaded with name " + name + " for type " + MACRO_TO_STRING(T));
			}
			return *mResources[id][name];
		}

		template <typename T> requires std::derived_from<T, Resource<T>> void unloadResource(const std::string& name) {
			const uint32_t id = T::getID();
			if (id >= mResources.size() || !mResources[id][name]) {
				throw std::runtime_error("No resource loaded with name " + name + " for type " + MACRO_TO_STRING(T));
			}
			mResources[id].erase(name);
		}

		template <typename T> requires std::derived_from<T, Resource<T>> void unloadResourceType() {
			const uint32_t id = T::getID();
			if (id < mResources.size()) {
				mResources[id].clear();
			}
		}

	private:
		std::vector<std::unique_ptr<IResourceLoader>> mResourceLoaders;
		std::vector<std::unordered_map<std::string, std::unique_ptr<resource::IResource>>> mResources;
		std::vector<std::string> mResourcePaths;
	};
}
