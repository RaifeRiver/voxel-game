#include "CommandQueue.h"

namespace voxel_game::ecs {
	void CommandQueue::pushCommand(const Command &command) {
		std::unique_lock lock(mMutex);
		mCommands.push(command);
	}

	void CommandQueue::execute(Registry &registry) {
		std::unique_lock lock(mMutex);
		while (!mCommands.empty()) {
			mCommands.front()(registry);
			mCommands.pop();
		}
	}
}