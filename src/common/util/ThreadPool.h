#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using Task = std::function<void()>;

class ThreadPool {
public:
	explicit ThreadPool(uint32_t threads);

	void queueTask(const Task &task);

	void destroy();

private:
	std::vector<std::thread> mThreads;
	std::mutex mMutex;
	std::queue<Task> mTasks;
	std::condition_variable mCondition;
	bool mRunning;

	void run();
};
