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

#pragma once

#include <optional>
#include <string>

#include "common/ecs/Resource.h"

namespace voxel_game::client {
	enum class RenderBackend {
		OPENGL,
		VULKAN
	};

	std::optional<bool> parseBool(const std::string& value);

	std::optional<uint32_t> parseUint32(const std::string& value, uint32_t min = 0, uint32_t max = UINT32_MAX);

	class LaunchOptions : public ecs::Resource<LaunchOptions> {
	public:
		LaunchOptions(int argc, char** argv);

		[[nodiscard]] RenderBackend getRenderBackend() const {
			return mRenderBackend.value();
		}

		[[nodiscard]] bool enableVsync() const {
			return mEnableVsync.value();
		}

		[[nodiscard]] uint32_t getLoadDistance() const {
			return mLoadDistance.value();
		}

	private:
		std::optional<RenderBackend> mRenderBackend;
		std::optional<bool> mEnableVsync;
		std::optional<uint32_t> mLoadDistance;
	};
}
