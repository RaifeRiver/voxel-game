#pragma once
#include "common/registry/RegistryObject.h"

namespace voxel_game::client::renderer::universe {
	class BlockMaterial : public registry::RegistryObject {
	public:
		explicit BlockMaterial(const std::string &texture);

		[[nodiscard]] const std::string& getTexture() const {
			return texture;
		}

	private:
		std::string texture;
	};
}
