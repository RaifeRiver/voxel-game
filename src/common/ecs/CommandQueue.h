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