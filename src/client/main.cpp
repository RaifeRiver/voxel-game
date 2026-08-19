#include "VoxelGameClient.h"
#include "common/VoxelGame.h"

int main(const int argc, char** argv) {
	const voxel_game::client::CommandLineArguments args(argc, argv);

	voxel_game::ecs::ECSRegistry registry;

	voxel_game::client::load(registry, args);
	voxel_game::load(registry);

	voxel_game::client::run(registry);

	voxel_game::destroy(registry);
	voxel_game::client::destroy(registry);
}
