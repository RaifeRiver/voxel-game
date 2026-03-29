#pragma once
#include <chrono>
#include <functional>

namespace voxel_game::util {
	struct ThreadPoolTask {
		int priority;
		std::chrono::steady_clock::time_point time;
		std::function<void()> task;

		bool operator<(const ThreadPoolTask &other) const {
			if (time != other.time) {
				return time < other.time;
			}
			return priority < other.priority;
		}
	};
}
