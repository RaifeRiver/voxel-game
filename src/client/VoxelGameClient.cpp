#include "VoxelGameClient.h"

namespace voxel_game::client {
	void VoxelGameClient::init() {
		sInstance = std::unique_ptr<VoxelGameClient>(new VoxelGameClient());
		sInstance->init_();
	}

	void VoxelGameClient::run() const {
		while (!mRenderEngine->getWindowManager()->shouldClose()) {
			mRenderEngine->render();
		}
	}

	void VoxelGameClient::destroy() const {
		mUniverseRenderer->destroy(mRenderEngine->getRenderContext());
		mRenderEngine->destroy();
		mResourceManager->destroy();
	}

	void VoxelGameClient::init_() {
		mResourceManager = std::make_unique<resource::ResourceManager>();
		mRenderEngine = std::make_unique<renderer::RenderEngine>();
		mUniverseRenderer = std::make_unique<renderer::universe::UniverseRenderer>();
		mUniverseRenderer->init(mRenderEngine->getRenderContext());
	}
}
