#include "RenderEngine.h"

namespace voxel_game::client::render::engine {
	RenderEngine::RenderEngine(ecs::ECSRegistry& registry) {
		initShaderCompiler(registry.getResource<resource::ResourceManager>());
	}
}
