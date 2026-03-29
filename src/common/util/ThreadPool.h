#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "ThreadPoolTask.h"

namespace voxel_game::util {
	class ThreadPool {
	public:
		explicit ThreadPool(int threads, std::string name);

		void scheduleTask(int priority, std::chrono::steady_clock::time_point time, const std::function<void()> &task);

		void scheduleTaskNow(int priority, const std::function<void()> &task);

		void destroy();

	private:
		std::vector<std::thread> mThreads;
		std::mutex mTaskMutex;
		std::priority_queue<ThreadPoolTask> mTasks;
		std::condition_variable mCondition;
		bool mStop = false;
		std::string mName;

		void workerThread(uint32_t id, int32_t group);
	};
}
