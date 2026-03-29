#include "VoxelGame.h"

#include "registry/Blocks.h"

namespace voxel_game {
	void VoxelGame::init() {
		if (sInstance != nullptr) {
			throw std::runtime_error("Already initialised");
		}

		sInstance = std::make_unique<VoxelGame>();
		sInstance->init_();
	}

	void VoxelGame::destroy() {

	}

	void VoxelGame::init_() {
		registry::blocks::BLOCKS.lock();
	}
}
