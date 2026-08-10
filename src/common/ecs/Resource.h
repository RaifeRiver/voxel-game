#pragma once

#include <cstdint>

namespace voxel_game::ecs {
	class IResource {
	public:
		virtual void destroy() {}

		virtual ~IResource() = default;
	};

	// ReSharper disable once CppTemplateParameterNeverUsed
	template <typename T> class Resource : public IResource {
	public:
		[[nodiscard]] static uint32_t getID() {
			static uint32_t id = sNextID++;
			return id;
		}

	private:
		static inline uint32_t sNextID = 0;
	};
}