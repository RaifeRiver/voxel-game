#include "Resource.h"

namespace voxel_game::resource {
	std::string to_string(const ResourceType type) {
		switch (type) {
			case ResourceType::ASSET:
				return "assets";
			case ResourceType::DATA:
				return "data";
			default:
				return "unknown";
		}
	}

	IResource::IResource(const ResourceType type) : mType(type) {}
}
