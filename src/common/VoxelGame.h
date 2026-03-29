#pragma once
#include <memory>

namespace voxel_game {
	class VoxelGame {
	public:
		static void init();

		[[nodiscard]] static VoxelGame * get() {
			return sInstance.get();
		}

		void destroy();

	private:
		static inline std::unique_ptr<VoxelGame> sInstance = nullptr;

		void init_();
	};
}
