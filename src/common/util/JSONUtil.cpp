#include "JSONUtil.h"

namespace voxel_game::util {
	static simdjson::dom::parser parser;

	simdjson::dom::element parseJSON(const std::string& file) {
		const simdjson::padded_string json = simdjson::padded_string::load(file);
		return parser.parse(json);
	}
}
