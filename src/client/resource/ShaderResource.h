#pragma once

#include <vulkan/vulkan_core.h>

#include "Resource.h"

namespace voxel_game::client::resource {
	class ShaderResource: public Resource {
	public:
		explicit ShaderResource(const std::string &name);

		void init(std::string path) override;

		[[nodiscard]] VkShaderModule getShaderModule() const {
			return mShaderModule;
		}

		void destroy() override;

	private:
		VkShaderModule mShaderModule = nullptr;
	};
}