#pragma once

namespace voxel_game::client {
	enum class RenderLibrary {
		OPENGL,
		VULKAN
	};

	class CommandLineArguments {
	public:
		CommandLineArguments(int argc, char** argv);

		[[nodiscard]] RenderLibrary getRenderLibrary() const {
			return mRenderLibrary;
		}

	private:
		RenderLibrary mRenderLibrary = RenderLibrary::VULKAN;
	};
}