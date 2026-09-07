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

#include <functional>
#include <mutex>
#include <queue>

namespace voxel_game::ecs {
	class ECSRegistry;

	using Command = std::function<void(ECSRegistry& registry)>;

	class CommandQueue {
	public:
		void pushCommand(const Command &command);

		void execute(ECSRegistry& registry);

	private:
		std::mutex mMutex;
		std::queue<Command> mCommands;
	};
}