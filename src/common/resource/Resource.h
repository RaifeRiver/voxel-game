#pragma once

#include <cstdint>

namespace voxel_game::resource {
    enum class ResourceType {
        ASSET,
        DATA
    };

	class IResource {
	public:
		explicit IResource(ResourceType type);

		[[nodiscard]] ResourceType getType() const {
			return mType;
		}

		virtual ~IResource() = default;

	protected:
		static inline uint32_t sNextID = 0;

	private:
		ResourceType mType;
	};

	// ReSharper disable once CppTemplateParameterNeverUsed
    template <typename T> class Resource : public IResource {
    public:
	    explicit Resource(const ResourceType type) : IResource(type) {}

    	[[nodiscard]] static uint32_t getID() {
	    	static uint32_t id = sNextID++;
	    	return id;
	    }
    };
}