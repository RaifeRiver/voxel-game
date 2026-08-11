#include "VoxelGameClient.h"
#include "common/VoxelGame.h"

int main() {
	voxel_game::ecs::ECSRegistry registry;

	voxel_game::client::load(registry);
	voxel_game::load(registry);

	voxel_game::client::run(registry);

	voxel_game::destroy(registry);
	voxel_game::client::destroy(registry);
}
