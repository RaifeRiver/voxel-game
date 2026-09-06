#include "RenderPipeline.h"

#include <stdexcept>
#include <utility>

namespace voxel_game::client::render::engine {
	RenderPipeline::RenderPipeline() : Pipeline(PipelineType::RENDER) {}

	RenderPipelineBuilder::RenderPipelineBuilder(Shader vertexShader, Shader fragmentShader) : mVertexShader(std::move(vertexShader)), mFragmentShader(std::move(fragmentShader)) {}

	RenderPipelineBuilder* RenderPipelineBuilder::primitiveTopology(const PrimitiveTopology primitiveTopology) {
		mPrimitiveTopology = primitiveTopology;
		return this;
	}

	RenderPipelineBuilder* RenderPipelineBuilder::polygonMode(const PolygonMode polygonMode) {
		mPolygonMode = polygonMode;
		return this;
	}

	RenderPipelineBuilder* RenderPipelineBuilder::lineWidth(const float lineWidth) {
		if (lineWidth <= 0.0f) {
			throw std::runtime_error("lineWidth must be greater than 0.0");
		}
		mLineWidth = lineWidth;
		return this;
	}

	RenderPipelineBuilder* RenderPipelineBuilder::cullMode(const CullMode cullMode, const FrontFace frontFace) {
		mCullMode = cullMode;
		mFrontFace = frontFace;
		return this;
	}

	RenderPipelineBuilder* RenderPipelineBuilder::colourFormat(const ImageFormat colourFormat) {
		if (!isColourFormat(colourFormat)) {
			throw std::runtime_error("colourFormat must be a colour format");
		}
		mColourFormat = colourFormat;
		return this;
	}

	RenderPipelineBuilder* RenderPipelineBuilder::depthFormat(const ImageFormat depthFormat) {
		if (!isDepthFormat(depthFormat) || depthFormat == ImageFormat::UNKNOWN) {
			throw std::runtime_error("depthFormat must be a depth format");
		}
		mDepthFormat = depthFormat;
		return this;
	}

	RenderPipelineBuilder * RenderPipelineBuilder::blendMode(const BlendMode blendMode) {
		mBlendMode = blendMode;
		return this;
	}
}
