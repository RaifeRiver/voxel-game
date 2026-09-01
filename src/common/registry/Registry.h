#pragma once

#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/ecs/Resource.h"

namespace voxel_game::registry {
	const auto VALID_REGISTRY_NAMES = std::regex("[a-z_][a-z_0-9]*:[a-z_][a-z_0-9]*");

	template <typename T> class Registry : public ecs::Resource<Registry<T>> {
	public:
		T& registerObject(const std::string& name, std::unique_ptr<T> object) {
			if (!std::regex_match(name, VALID_REGISTRY_NAMES)) {
				throw std::runtime_error("Invalid registry name: " + name);
			}
			const uint32_t id = mObjects.size();
			mObjects.push_back(object);
			mNumericIDs[name] = id;
			return *mObjects.back();
		}

		template <typename U, typename... Args> T& registerNew(const std::string& name, Args... args) {
			if (!std::regex_match(name, VALID_REGISTRY_NAMES)) {
				throw std::runtime_error("Invalid registry name: " + name);
			}
			const uint32_t id = mObjects.size();
			mObjects.push_back(std::make_unique<U>((std::forward<Args>(args), ...)));
			mNumericIDs[name] = id;
			return *mObjects.back();
		}

		[[nodiscard]] T& operator[](const uint32_t id) const {
			return *mObjects[id];
		}

		[[nodiscard]] T& operator[](const std::string& name) {
			return *mObjects[mNumericIDs[name]];
		}

		[[nodiscard]] const std::vector<std::unique_ptr<T>>& getObjects() const {
			return mObjects;
		}

	private:
		std::unordered_map<std::string, uint32_t> mNumericIDs;
		std::vector<std::unique_ptr<T>> mObjects;
	};
}
