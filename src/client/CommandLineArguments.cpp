#include "CommandLineArguments.h"

#include <cstring>
#include <stdexcept>

#include "common/util/Log.h"

namespace voxel_game::client {
	CommandLineArguments::CommandLineArguments(const int argc, char** argv) {
		bool renderLibrarySet = false;
		for (int i = 1; i < argc; i++) {
			const char* arg = argv[i];
			if (strcmp(arg, "--opengl") == 0) {
				if (renderLibrarySet) {
					LOG_FATAL("Only one render library may be specified");
					throw std::runtime_error("Only one render library may be specified");
				}
				renderLibrarySet = true;
				mRenderLibrary = RenderLibrary::OPENGL;
			}
			else if (strcmp(arg, "--vulkan") == 0) {
				if (renderLibrarySet) {
					LOG_FATAL("Only one render library may be specified");
					throw std::runtime_error("Only one render library may be specified");
				}
				renderLibrarySet = true;
				mRenderLibrary = RenderLibrary::VULKAN;
			}
			else {
				LOG_FATAL("Unknown argument: {}", arg);
				throw std::runtime_error("Unknown argument: " + std::string(arg));
			}
		}
	}
}
