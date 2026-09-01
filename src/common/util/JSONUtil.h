#pragma once

#include "simdjson/simdjson.h"

namespace voxel_game::util {
	simdjson::dom::element parseJSON(const std::string& file);
}
