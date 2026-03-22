#pragma once

#include <memory>

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

		[[nodiscard]] renderer::RenderEngine * getRenderEngine() const {
			return mRenderEngine.get();
		}

		[[nodiscard]] resource::ResourceManager * getResourceManager() const {
			return mResourceManager.get();
		}

		[[nodiscard]] renderer::universe::UniverseRenderer * getUniverseRenderer() const {
			return mUniverseRenderer.get();
		}

		void run() const;

		void destroy() const;

	private:
		static inline std::unique_ptr<VoxelGameClient> sInstance = nullptr;

		std::unique_ptr<renderer::RenderEngine> mRenderEngine;
		std::unique_ptr<resource::ResourceManager> mResourceManager;
		std::unique_ptr<renderer::universe::UniverseRenderer> mUniverseRenderer;

		void init_();
	};
}
