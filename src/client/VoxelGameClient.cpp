#include "VoxelGameClient.h"

namespace voxel_game::client {
	void VoxelGameClient::init() {
		sInstance = std::unique_ptr<VoxelGameClient>(new VoxelGameClient());
	}

	void VoxelGameClient::run() const {
		while (!mRenderEngine->getWindowManager()->shouldClose()) {
			mRenderEngine->render();
		}
	}

	void VoxelGameClient::destroy() const {
		mRenderEngine->destroy();
	}

	VoxelGameClient::VoxelGameClient() {
		mRenderEngine = std::make_unique<renderer::RenderEngine>();
	}
}
