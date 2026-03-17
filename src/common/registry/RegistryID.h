#pragma once

#include <regex>
#include <string>

namespace voxel_game::registry {
	static const std::regex VALID_REGISTRY_IDS{"^[a-z0-9_]+:[a-z0-9_/]+$"};

	struct RegistryID {

		std::string namespace_;
		std::string id;

		[[nodiscard]] std::string getID() const;

		bool operator==(const RegistryID &other) const;

		static RegistryID create(const std::string& id) {
			if (!std::regex_match(id, VALID_REGISTRY_IDS)) {
				throw std::invalid_argument("Invalid registry id");
			}
			return RegistryID{id.substr(0, id.find(':')), id.substr(id.find(':') + 1)};
		}
	};

	struct RegistryIDHash {
		size_t operator()(const RegistryID& id) const noexcept {
			const size_t namespaceHash = std::hash<std::string>{}(id.namespace_);
			const size_t idHash = std::hash<std::string>{}(id.id);
			return namespaceHash ^ idHash + 0x9e3779b9 + (namespaceHash << 6) + (namespaceHash >> 2);
		}
	};
}
