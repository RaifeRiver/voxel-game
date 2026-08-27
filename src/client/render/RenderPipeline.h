#pragma once

#include <memory>
#include <string>

#include "GPUImage.h"
#include "Pipeline.h"

namespace voxel_game::client::render {
	enum class PrimitiveTopology {
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
		TRIANGLE_FAN
	};

	enum class PolygonMode {
		FILL,
		LINE,
		POINT
	};

	enum class CullMode {
		NONE,
		FRONT,
		BACK,
		FRONT_AND_BACK
	};

	enum class FrontFace {
		COUNTER_CLOCKWISE,
		CLOCKWISE
	};

	class RenderPipeline : public Pipeline {
	public:
		RenderPipeline();

		void draw(const uint32_t vertexCount, const uint32_t firstVertex = 0) {
			draw_(vertexCount, firstVertex);
		}

	protected:
		virtual void draw_(uint32_t vertexCount, uint32_t firstVertex) = 0;
	};

	class RenderPipelineBuilder {
	public:
		RenderPipelineBuilder(std::string vertexShader, std::string fragmentShader);

		[[nodiscard]] std::string getVertexShader() const {
			return mVertexShader;
		}

		[[nodiscard]] std::string getFragmentShader() const {
			return mFragmentShader;
		}

		[[nodiscard]] PrimitiveTopology getPrimitiveTopology() const {
			return mPrimitiveTopology;
		}

		RenderPipelineBuilder* primitiveTopology(PrimitiveTopology primitiveTopology);

		[[nodiscard]] PolygonMode getPolygonMode() const {
			return mPolygonMode;
		}

		RenderPipelineBuilder* polygonMode(PolygonMode polygonMode);

		[[nodiscard]] float getLineWidth() const {
			return mLineWidth;
		}

		RenderPipelineBuilder* lineWidth(float lineWidth);

		[[nodiscard]] CullMode getCullMode() const {
			return mCullMode;
		}

		[[nodiscard]] FrontFace getFrontFace() const {
			return mFrontFace;
		}

		RenderPipelineBuilder* cullMode(CullMode cullMode, FrontFace frontFace = FrontFace::COUNTER_CLOCKWISE);

		[[nodiscard]] ImageFormat getColourFormat() const {
			return mColourFormat;
		}

		RenderPipelineBuilder* colourFormat(ImageFormat colourFormat);

		[[nodiscard]] ImageFormat getDepthFormat() const {
			return mDepthFormat;
		}

		RenderPipelineBuilder* depthFormat(ImageFormat depthFormat);

		virtual std::unique_ptr<RenderPipeline> build() = 0;

		virtual ~RenderPipelineBuilder() = default;

	protected:
		std::string mVertexShader;
		std::string mFragmentShader;
		PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		PolygonMode mPolygonMode = PolygonMode::FILL;
		float mLineWidth = 1.0f;
		CullMode mCullMode = CullMode::NONE;
		FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
		ImageFormat mColourFormat = ImageFormat::RGBA16_SFLOAT;
		ImageFormat mDepthFormat = ImageFormat::UNKNOWN;
	};
}
