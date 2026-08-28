#pragma once

#include "DescriptorSet.h"

namespace voxel_game::client::render {
    enum class PipelineType {
        COMPUTE,
    	RENDER
    };

    class Pipeline {
    public:
        explicit Pipeline(PipelineType type);

    	virtual void bind() = 0;

    	virtual void bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) = 0;

    	virtual void setPushConstants(void* pushConstants) = 0;

        [[nodiscard]] PipelineType getType() const {
	        return mType;
        }

        virtual ~Pipeline() = default;

    private:
        PipelineType mType;
    };
}
