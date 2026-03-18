#include "VoxelGameClient.h"

using namespace voxel_game::client;

int main() {
	VoxelGameClient::init();
	VoxelGameClient* client = VoxelGameClient::getClient();
	client->run();
	client->destroy();
}
