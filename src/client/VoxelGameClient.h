#pragma once

#include <memory>

#include "renderer/RenderEngine.h"

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

		void run() const;

		void destroy() const;

	private:
		static inline std::unique_ptr<VoxelGameClient> sInstance = nullptr;

		std::unique_ptr<renderer::RenderEngine> mRenderEngine;

		VoxelGameClient();
	};
}
