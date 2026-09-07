/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
