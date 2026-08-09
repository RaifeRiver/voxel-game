#pragma once

#include <functional>
#include <mutex>
#include <queue>

class Registry;

using Command = std::function<void(Registry& registry)>;

class CommandQueue {
public:
	void pushCommand(const Command &command);

	void execute(Registry& registry);

private:
	std::mutex mMutex;
	std::queue<Command> mCommands;
};