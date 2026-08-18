#pragma once

namespace voxel_game::client::render {
    enum class PipelineType {
        COMPUTE
    };

    class Pipeline {
    public:
        explicit Pipeline(PipelineType type);

        virtual ~Pipeline() = default;

        [[nodiscard]] PipelineType getType() const {
            return mType;
        }

    private:
        PipelineType mType;
    };
}