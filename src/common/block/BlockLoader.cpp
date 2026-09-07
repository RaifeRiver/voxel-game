/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "BlockLoader.h"

#include "Block.h"
#include "common/component/TextID.h"
#include "common/resource/ResourceManager.h"
#include "common/util/JSONUtil.h"
#include "common/util/Log.h"

namespace voxel_game::block {
	void loadBlocks(ecs::ECSRegistry& registry) {
		const resource::ResourceManager& resourceManager = registry.getResource<resource::ResourceManager>();
		const std::vector<resource::FoundResource> resources = resourceManager.findResources("blocks", ".json", resource::ResourceType::DATA);
		for (const auto&[namespace_, name, path] : resources) {
			simdjson::dom::element json = util::parseJSON(path);
			if (!json.is_object()) {
				LOG_ERROR("Error loading block file: block definition must be an object: {}:{}", namespace_, name);
				continue;
			}
			const ecs::Entity entity = registry.createEntity();
			try {
				for (const simdjson::dom::key_value_pair& field : json.get_object()) {
					registry.attachComponent(entity, std::string(field.key)).loadFromJSON(field.value);
				}
			}
			catch (const std::exception& e) {
				LOG_ERROR("Error loading block file: {}:{}, {}", namespace_, name, e.what());
				registry.destroyEntity(entity);
				continue;
			}
			registry.attachComponent<Block>(entity);
			auto& textID = registry.attachComponent<component::TextID>(entity);
			textID.namespace_ = namespace_;
			textID.name = name;
			LOG_DEBUG("Loaded block: {}:{}", namespace_, name);
		}
	}
}
