#include "ThreadPool.h"

ThreadPool::ThreadPool(const uint32_t threads) {
	mRunning = true;

	for (uint32_t i = 0; i < threads; ++i) {
		mThreads.emplace_back(&ThreadPool::run, this);
	}
}

void ThreadPool::queueTask(const Task &task) {
	std::unique_lock lock(mMutex);
	mTasks.push(task);
	lock.unlock();
	mCondition.notify_all();
}

void ThreadPool::destroy() {
	mRunning = false;
	mCondition.notify_all();
	for (std::thread& thread : mThreads) {
		thread.join();
	}
}

void ThreadPool::run() {
	while (mRunning) {
		std::unique_lock lock(mMutex);
		if (mTasks.empty()) {
			mCondition.wait(lock, [this] {
				return !mRunning || !mTasks.empty();
			});
		}
		if (!mRunning) {
			return;
		}

		Task task = mTasks.front();
		mTasks.pop();
		lock.unlock();
		task();
	}
}
