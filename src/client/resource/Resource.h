#pragma once

#include <string>

namespace voxel_game::client::resource {
	class Resource {
	public:
		explicit Resource(const std::string &name);

		virtual void init(std::string path) = 0;

		virtual void destroy() = 0;

		[[nodiscard]] std::string getName() const {
			return mName;
		}

		virtual ~Resource() = default;
	private:
		std::string mName;
	};
}
