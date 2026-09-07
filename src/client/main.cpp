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

#include "VoxelGameClient.h"
#include "common/VoxelGame.h"
#include "common/util/Log.h"

int main(const int argc, char** argv) {
	LOG_INFO("Loading Voxel Game");
	const std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

	const voxel_game::client::CommandLineArguments args(argc, argv);

	voxel_game::ecs::ECSRegistry registry;

	voxel_game::preLoad(registry);
	voxel_game::client::load(registry, args);
	voxel_game::load(registry);
	voxel_game::postLoad(registry);

	LOG_INFO("Loaded Voxel Game in {}s", std::chrono::duration<float>(std::chrono::round<std::chrono::duration<uint32_t, std::ratio<1, 10>>>(std::chrono::steady_clock::now() - startTime)).count());

	voxel_game::client::run(registry);

	LOG_INFO("Stopping Voxel Game");

	voxel_game::destroy(registry);
	voxel_game::client::destroy(registry);
}
