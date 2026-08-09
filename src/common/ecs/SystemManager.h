#pragma once

#include <functional>

class Registry;

using System = std::function<void(Registry&, float)>;

enum class Stage {
	PRE_UPDATE,
	BLOCK_UPDATE,
	ENTITY_UPDATE,
	PHYSICS,
	POST_UPDATE,
	WORLD_RENDER,
	UI_RENDER,
	COUNT
};

class SystemManager {
public:
	void registerSystem(Stage stage, const System &system);

	void runStage(Stage stage, Registry& registry, float deltaTime);

private:
	std::vector<System> mStages[static_cast<size_t>(Stage::COUNT)];
};