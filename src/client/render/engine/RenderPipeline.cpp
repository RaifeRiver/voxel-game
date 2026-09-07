/*
 * Voxel Game
 * Copyright (C) 2026 RaifeRiver
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
