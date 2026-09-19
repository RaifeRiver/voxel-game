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

#include "LaunchOptions.h"

#include <stdexcept>

#include "common/util/Log.h"
#include "common/util/OptionalUtil.h"

namespace voxel_game::client {
	std::optional<bool> parseBool(const std::string& value) {
		if (value == "true") {
			return true;
		}
		if (value == "false") {
			return false;
		}
		LOG_ERROR("Invalid value for boolean argument: {}", value);
		return {};
	}

	std::optional<uint32_t> parseUint32(const std::string& value, const uint32_t min, const uint32_t max) {
		try {
			long long i = std::stoll(value);
			if (i > max) {
				LOG_ERROR("Value for integer argument is too large: {}", i);
				return max;
			}
			if (i < min) {
				LOG_ERROR("Value for integer argument is too small: {}", i);
				return min;
			}
			return i;
		}
		catch (const std::invalid_argument&) {
			LOG_ERROR("Invalid value for integer argument: {}", value);
			return {};
		}
	}

	LaunchOptions::LaunchOptions(const int argc, char** argv) {
		for (size_t i = 1; i < argc; i++) {
			std::string arg = argv[i];
			const size_t equalsPos = arg.find('=');
			std::string name = "";
			std::string value = "";
			const bool hasValue = equalsPos != std::string::npos;
			if (hasValue) {
				name = arg.substr(0, equalsPos);
				value = arg.substr(equalsPos + 1);
			}
			else {
				name = arg;
			}
			if (name == "--renderer") {
				if (mRenderBackend) {
					LOG_ERROR("Argument --renderer can only be specified once");
				}
				else if (!hasValue) {
					LOG_ERROR("Argument --renderer requires value, ignoring");
				}
				else if (value == "opengl") {
					mRenderBackend = RenderBackend::OPENGL;
				}
				else if (value == "vulkan") {
					mRenderBackend = RenderBackend::VULKAN;
				}
				else {
					LOG_ERROR("Unknown value for argument --renderer, ignoring: {}", value);
				}
			}
			else if (name == "--vsync") {
				if (mEnableVsync) {
					LOG_ERROR("Argument --vsync can only be specified once");
				}
				else if (hasValue) {
					mEnableVsync = parseBool(value);
				}
				else {
					mEnableVsync = true;
				}
			}
			else if (name == "--load-distance") {
				if (mLoadDistance) {
					LOG_ERROR("Argument --load-distance can only be specified once");
				}
				else if (!hasValue) {
					LOG_ERROR("Argument --load-distance requires value, ignoring");
				}
				else {
					mLoadDistance = parseUint32(value, 1);
				}
			}
			else if (name == "--gpu") {
				if (mGPU) {
					LOG_ERROR("Argument --gpu can only be specified once");
				}
				else if (!hasValue) {
					LOG_ERROR("Argument --gpu requires value, ignoring");
				}
				else {
					mGPU = parseUint32(value, 0);
				}
			}
			else {
				LOG_ERROR("Unknown argument, ignoring: {}", arg);
			}
		}

		util::optional::setDefault(mRenderBackend, RenderBackend::VULKAN);
		util::optional::setDefault(mEnableVsync, true);
		util::optional::setDefault(mLoadDistance, 8u);
	}
}
