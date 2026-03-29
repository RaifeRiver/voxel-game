#include "tracy/Tracy.hpp"

#include "VoxelGameClient.h"

using namespace voxel_game::client;

int main() {
	while (!TracyIsConnected) {}

	tracy::SetThreadName("Main thread");

	VoxelGameClient::init();
	VoxelGameClient* client = VoxelGameClient::getClient();
	client->run();
	client->destroy();
}
