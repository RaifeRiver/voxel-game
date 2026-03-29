#include "ThreadPool.h"

#include <iostream>
#include <random>
#include <utility>

#include "tracy/Tracy.hpp"

namespace voxel_game::util {
	ThreadPool::ThreadPool(const int threads, std::string name) : mName(std::move(name)) {
		int32_t group = std::rand();
		for (int i = 0; i < threads; i++) {
			mThreads.emplace_back(&ThreadPool::workerThread, this, i, group);
		}
	}

	void ThreadPool::scheduleTask(const int priority, const std::chrono::steady_clock::time_point time, const std::function<void()> &task) {
		std::unique_lock lock(mTaskMutex);
		mTasks.push({priority, time, task});
		lock.unlock();
		mCondition.notify_all();
	}

	void ThreadPool::scheduleTaskNow(const int priority, const std::function<void()> &task) {
		std::unique_lock lock(mTaskMutex);
		mTasks.push({priority, std::chrono::steady_clock::now(), task});
		lock.unlock();
		mCondition.notify_all();
	}

	void ThreadPool::destroy() {
		mStop = true;
		mCondition.notify_all();
		for (std::thread &thread : mThreads) {
			thread.join();
		}
	}

	void ThreadPool::workerThread(uint32_t id, const int32_t group) {
		tracy::SetThreadNameWithHint(std::format("{} pool worker #{}", mName, id).c_str(), group);

		while (true) {
			std::unique_lock lock(mTaskMutex);
			mCondition.wait(lock, [this] {
				return mStop || !mTasks.empty();
			});
			if (mStop && mTasks.empty()) {
				return;
			}

			std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
			const ThreadPoolTask& nextTask = mTasks.top();
			if (nextTask.time > now) {
				mCondition.wait_until(lock, nextTask.time);
			}
			else {
				ThreadPoolTask task = std::move(const_cast<ThreadPoolTask&>(nextTask));
				mTasks.pop();
				lock.unlock();
				task.task();
			}
		}
	}
}
