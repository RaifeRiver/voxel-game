#include "Profiler.h"

#include <fstream>

#include "FileHelper.h"

namespace voxel_game::util::profiler {
	namespace {
		std::vector<Frame> frames;
		Region* currentRegion;
		Frame* currentFrame;
	}

	void init() {
		std::atexit([] {
			endFrame();
		});
	}

	void beginFrame() {
		currentFrame = &frames.emplace_back();
		frames.back().start = std::chrono::steady_clock::now();
	}

	void beginRegion(const std::string& label) {
		if (!currentFrame) {
			throw std::runtime_error("beginRegion() called outside frame");
		}
		if (currentRegion) {
			currentRegion->children.push_back({.label = label, .parent = currentRegion});
			Region& region = currentRegion->children.back();
			currentRegion = &region;
			region.start = std::chrono::steady_clock::now();
		}
		else {
			currentFrame->regions.push_back({.label = label, .parent = currentRegion});
			Region& region = currentFrame->regions.back();
			currentRegion = &region;
			region.start = std::chrono::steady_clock::now();
		}
	}

	void endRegion() {
		const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		if (!currentFrame || !currentRegion) {
			throw std::runtime_error("endRegion() called without beginRegion()");
		}
		currentRegion->end = end;
		currentRegion = currentRegion->parent;
	}

	void endFrame() {
		const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		if (!currentFrame) {
			throw std::runtime_error("endFrame() called without beginFrame()");
		}
		while (currentRegion) {
			endRegion();
		}
		currentFrame->end = end;
		currentFrame = nullptr;
	}

	const std::vector<Frame>& getFrames() {
		return frames;
	}

	static void saveRegion(std::ofstream& file, const Region& region) {
		writeToFile(file, region.label.size());
		file.write(region.label.c_str(), region.label.size());
		writeToFile(file, std::chrono::duration_cast<std::chrono::nanoseconds>(region.start.time_since_epoch()).count());
		writeToFile(file, std::chrono::duration_cast<std::chrono::nanoseconds>(region.end.time_since_epoch()).count());
		writeToFile(file, region.children.size());
		for (const Region& child: region.children) {
			saveRegion(file, child);
		}
	}

	void saveData() {
		std::ofstream file(std::format("voxel_game_profile_{:%Y_%m_%d_%H_%M_%S}.bin", std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now())), std::ios::binary);
		writeToFile(file, frames.size());
		for (const auto& [regions, start, end]: frames) {
			writeToFile(file, std::chrono::duration_cast<std::chrono::nanoseconds>(start.time_since_epoch()).count());
			writeToFile(file, std::chrono::duration_cast<std::chrono::nanoseconds>(end.time_since_epoch()).count());
			writeToFile(file, regions.size());
			for (const Region& region: regions) {
				saveRegion(file, region);
			}
		}
		file.close();
	}
}
