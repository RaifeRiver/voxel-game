#pragma once

#include "Pipeline.h"

namespace voxel_game::client::render {
	class ComputePipeline : public Pipeline {
	public:
		ComputePipeline();

		void dispatch(const uint32_t x, const uint32_t y = 1, const uint32_t z = 1) {
			dispatch_(x, y, z);
		}

	protected:
		virtual void dispatch_(uint32_t x, uint32_t y, uint32_t z) = 0;
	};
}
