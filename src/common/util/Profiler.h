#pragma once

#include <chrono>
#include <string>

namespace voxel_game::util::profiler {
	struct Region {
		std::string label;
		std::chrono::steady_clock::time_point start;
		std::chrono::steady_clock::time_point end;
		std::vector<Region> children;
		Region* parent;
	};

	struct Frame {
		std::vector<Region> regions;
		std::chrono::steady_clock::time_point start;
		std::chrono::steady_clock::time_point end;
	};

	void init();

	void beginFrame();

	void beginRegion(const std::string& label);

	void endRegion();

	void endFrame();

	const std::vector<Frame>& getFrames();

	void saveData();
}
