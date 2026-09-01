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
