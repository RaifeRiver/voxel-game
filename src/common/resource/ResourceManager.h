#pragma once

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
		explicit IResourceLoader(const std::vector<std::string>& extensions);

		virtual std::unique_ptr<IResource> loadResource(const std::string& path) = 0;

		[[nodiscard]] const std::vector<std::string>& getExtensions() const {
			return mExtensions;
		}

		virtual ~IResourceLoader() = default;

	private:
		std::vector<std::string> mExtensions;
	};

	template <typename T> requires std::derived_from<T, Resource<T>> class ResourceLoader : public IResourceLoader {
	public:
		explicit ResourceLoader(const std::vector<std::string>& extensions) : IResourceLoader(extensions) {}

		std::unique_ptr<T> loadResource(const std::string& path) override = 0;
	};

	struct FindResourceResult {
		bool found;
		std::string path;
	};

	class ResourceManager : public ecs::Resource<ResourceManager> {
	public:
		ResourceManager();

		[[nodiscard]] FindResourceResult findResource(std::string name, const std::string& extension, ResourceType type) const;

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
			mResourceLoaders[id].push_back(std::unique_ptr(loader));
		}

		template <typename T> requires std::derived_from<T, Resource<T>> T& loadResource(const std::string& name) {
#if VG_SIDE != CLIENT
			if (T::getType() == ResourceType::ASSET) {
				throw std::runtime_error(std::string("Asset for type ") + MACRO_TO_STRING(T) + " added on invalid side " + MACRO_TO_STRING(VG_SIDE));
			}
#endif
			const uint32_t id = T::getID();
			if (id >= mResourceLoaders.size() || mResourceLoaders[id].empty()) {
				throw std::runtime_error(std::string("No resource loader for type ") + MACRO_TO_STRING(T));
			}
			if (id >= mResources.size()) {
				mResources.resize(id + 1);
			}
			FindResourceResult findResourceResult = {
				.found = false,
			};
			IResourceLoader* resourceLoader = nullptr;
			for (const std::unique_ptr<IResourceLoader>& loader : mResourceLoaders[id]) {
				for (const std::string& extension : loader->getExtensions()) {
					const FindResourceResult result = findResource(name, extension, T::getType());
					if (result.found) {
						findResourceResult = result;
						resourceLoader = loader.get();
						goto found;
					}
				}
			}
			found:
			if (!findResourceResult.found) {
				throw std::runtime_error("No resource with name " + name + " for type " + MACRO_TO_STRING(T) + " was found");
			}
			mResources[id][name] = std::move(resourceLoader->loadResource(findResourceResult.path));
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

		[[nodiscard]] std::vector<std::string>& getResourcePaths() {
			return mResourcePaths;
		}

	private:
		std::vector<std::vector<std::unique_ptr<IResourceLoader>>> mResourceLoaders;
		std::vector<std::unordered_map<std::string, std::unique_ptr<resource::IResource>>> mResources;
		std::vector<std::string> mResourcePaths;
	};
}
