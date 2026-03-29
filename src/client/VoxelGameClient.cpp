#include "VoxelGameClient.h"

#include <memory>

#include "tracy/Tracy.hpp"

#include "common/VoxelGame.h"
#include "registry/BlockMaterials.h"
#include "tracy/TracyC.h"

namespace voxel_game::client {
	void VoxelGameClient::init() {
		if (sInstance != nullptr) {
			throw std::runtime_error("Already initialised");
		}

		sInstance = std::make_unique<VoxelGameClient>();
		sInstance->init_();
	}

	void VoxelGameClient::run() {
		while (!mRenderEngine->getWindowManager()->shouldClose()) {
			std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
			float deltaTime = std::chrono::duration<float>(currentTime - mLastFrameTime).count();
			mLastFrameTime = currentTime;
			mCamera->update(deltaTime);

			mRenderEngine->render();
			FrameMark;
		}
	}

	void VoxelGameClient::destroy() const {
		mThreadPool->destroy();

		mUniverseRenderer->destroy(mRenderEngine->getRenderContext());
		mResourceManager->destroy();
		mRenderEngine->destroy();

		VoxelGame::get()->destroy();
	}

	void VoxelGameClient::init_() {
		VoxelGame::init();

		ZoneScopedN("Client init");

		mThreadPool = std::make_unique<util::ThreadPool>(4, "Client");
		mResourceManager = std::make_unique<resource::ResourceManager>();

		registry::block_materials::BLOCK_MATERIALS.lock();

		mUniverse = std::make_unique<universe::Universe>();

		TracyCZoneN(initRenderEngine, "Initialise render engine", 1);
		mRenderEngine = std::make_unique<renderer::RenderEngine>();
		TracyCZoneEnd(initRenderEngine);
		TracyCZoneN(initUniverseRenderer, "Initialise universe renderer", 1);
		mUniverseRenderer = std::make_unique<renderer::universe::UniverseRenderer>();
		mUniverseRenderer->init(mRenderEngine->getRenderContext());
		TracyCZoneEnd(initUniverseRenderer);

		mCamera = std::make_unique<renderer::Camera>();

		TracyCZoneN(createWorld, "Create world", 1);
		auto *object = new universe::block::BlockObject();
		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				for (int z = 0; z < 8; z++) {
					object->loadChunk({x + 9, y, z});
					universe::block::Chunk *chunk = object->getChunk({x + 9, y, z});
					for (int x2 = 0; x2 < 32; x2++) {
						for (int y2 = 0; y2 < 32; y2++) {
							for (int z2 = 0; z2 < 32; z2++) {
								if (y2 != 28) {
									chunk->setBlock({x2, y2, z2}, 1);
								}
							}
						}
					}
				}
			}
		}

		glm::ivec3 chunkPos = {9 * 32, 0, 0};
		mUniverseRenderer->getChunks().push_back(std::make_unique<renderer::universe::SVDAG>(mUniverseRenderer.get(), object, chunkPos));

		TracyCZoneEnd(createWorld);

		mLastFrameTime = std::chrono::steady_clock::now();
	}
}
