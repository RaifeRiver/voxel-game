#include "CommandLineArguments.h"

#include <cstring>
#include <stdexcept>

namespace voxel_game::client {
	CommandLineArguments::CommandLineArguments(const int argc, char** argv) {
		bool renderLibrarySet = false;
		for (int i = 1; i < argc; i++) {
			const char* arg = argv[i];
			if (strcmp(arg, "--opengl") == 0) {
				if (renderLibrarySet) {
					throw std::runtime_error("Only one render library may be specified");
				}
				renderLibrarySet = true;
				mRenderLibrary = RenderLibrary::OPENGL;
			}
			else if (strcmp(arg, "--vulkan") == 0) {
				if (renderLibrarySet) {
					throw std::runtime_error("Only one render library may be specified");
				}
				renderLibrarySet = true;
				mRenderLibrary = RenderLibrary::VULKAN;
			}
			else {
				throw std::runtime_error("Unknown argument: " + std::string(arg));
			}
		}
	}
}
