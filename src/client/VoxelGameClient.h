#pragma once

#include <memory>

#include "common/universe/Universe.h"
#include "common/util/ThreadPool.h"
#include "renderer/Camera.h"
#include "renderer/RenderEngine.h"
#include "renderer/universe/UniverseRenderer.h"
#include "resource/ResourceManager.h"

namespace voxel_game::client {
	class VoxelGameClient {
	public:
		static void init();

		[[nodiscard]] static VoxelGameClient * getClient() {
			return sInstance.get();
		}

		[[nodiscard]] util::ThreadPool * getThreadPool() const {
			return mThreadPool.get();
		}

		[[nodiscard]] renderer::RenderEngine * getRenderEngine() const {
			return mRenderEngine.get();
		}

		[[nodiscard]] resource::ResourceManager * getResourceManager() const {
			return mResourceManager.get();
		}

		[[nodiscard]] renderer::universe::UniverseRenderer * getUniverseRenderer() const {
			return mUniverseRenderer.get();
		}

		[[nodiscard]] universe::Universe * getUniverse() const {
			return mUniverse.get();
		}

		[[nodiscard]] renderer::Camera * getCamera() const {
			return mCamera.get();
		}

		void run();

		void destroy() const;

	private:
		static inline std::unique_ptr<VoxelGameClient> sInstance = nullptr;

		std::unique_ptr<util::ThreadPool> mThreadPool;
		std::unique_ptr<renderer::RenderEngine> mRenderEngine;
		std::unique_ptr<resource::ResourceManager> mResourceManager;
		std::unique_ptr<renderer::universe::UniverseRenderer> mUniverseRenderer;
		std::unique_ptr<universe::Universe> mUniverse;
		std::unique_ptr<renderer::Camera> mCamera;
		std::chrono::steady_clock::time_point mLastFrameTime;

		void init_();
	};
}
