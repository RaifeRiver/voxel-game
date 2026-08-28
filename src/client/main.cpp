#include "VoxelGameClient.h"
#include "common/VoxelGame.h"
#include "common/util/Log.h"

int main(const int argc, char** argv) {
	LOG_INFO("Loading Voxel Game");
	const std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

	const voxel_game::client::CommandLineArguments args(argc, argv);

	voxel_game::ecs::ECSRegistry registry;

	voxel_game::preLoad(registry);
	voxel_game::client::load(registry, args);
	voxel_game::load(registry);

	LOG_INFO("Loaded Voxel Game in {}s", std::chrono::duration<float>(std::chrono::round<std::chrono::duration<uint32_t, std::ratio<1, 10>>>(std::chrono::steady_clock::now() - startTime)).count());

	voxel_game::client::run(registry);

	LOG_INFO("Stopping Voxel Game");

	voxel_game::destroy(registry);
	voxel_game::client::destroy(registry);
}
