#include "VoxelGameClient.h"
#include "common/VoxelGame.h"

int main() {
	voxel_game::ecs::ECSRegistry registry;

	voxel_game::client::load(registry);

	voxel_game::load(registry);
}
