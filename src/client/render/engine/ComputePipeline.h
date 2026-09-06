#pragma once

#include <string>

#include "Pipeline.h"

namespace voxel_game::client::render::engine {
	class ComputePipeline : public Pipeline {
	public:
		ComputePipeline();

		void dispatch(const uint32_t x, const uint32_t y = 1, const uint32_t z = 1, const std::string& label = "Unknown compute pipeline") {
			dispatch_(x, y, z, label);
		}

	protected:
		virtual void dispatch_(uint32_t x, uint32_t y, uint32_t z, const std::string& label) = 0;
	};
}
