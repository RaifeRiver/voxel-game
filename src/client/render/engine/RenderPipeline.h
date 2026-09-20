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

#pragma once

#include <memory>

#include "DescriptorLayout.h"
#include "GPUImage.h"
#include "Pipeline.h"
#include "Shader.h"

namespace voxel_game::client::render::engine {
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

	enum class BlendMode {
		NONE,
		ADDITIVE,
		ALPHA
	};

	class RenderPipeline : public Pipeline {
	public:
		RenderPipeline();

		virtual void bindIndexBuffer(GPUBuffer* buffer) = 0;

		void draw(const uint32_t vertexCount, const uint32_t firstVertex = 0, const std::string& label = "Unknown render pipeline") {
			draw_(vertexCount, firstVertex, label);
		}

		void drawIndexed(const uint32_t indexCount, const uint32_t firstIndex = 0, const std::string& label = "Unknown render pipeline") {
			drawIndexed_(indexCount, firstIndex, label);
		}

		void drawIndirectCount(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, const uint32_t maxCount, const std::string& label = "Unknown render pipeline") {
			drawIndirectCount_(indirectCommandBuffer, countBuffer, maxCount, 0, 0, label);
		}

		void drawIndirectCount(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, const uint32_t maxCount, const uint32_t commandOffset, const uint32_t countOffset = 0, const std::string& label = "Unknown render pipeline") {
			drawIndirectCount_(indirectCommandBuffer, countBuffer, maxCount, commandOffset, countOffset, label);
		}

		void drawIndexedIndirectCount(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, const uint32_t maxCount, const std::string& label = "Unknown render pipeline") {
			drawIndexedIndirectCount_(indirectCommandBuffer, countBuffer, maxCount, 0, 0, label);
		}

		void drawIndexedIndirectCount(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, const uint32_t maxCount, const uint32_t commandOffset, const uint32_t countOffset = 0, const std::string& label = "Unknown render pipeline") {
			drawIndexedIndirectCount_(indirectCommandBuffer, countBuffer, maxCount, commandOffset, countOffset, label);
		}

	protected:
		virtual void draw_(uint32_t vertexCount, uint32_t firstVertex, const std::string& label) = 0;

		virtual void drawIndexed_(uint32_t indexCount, uint32_t firstIndex, const std::string& label) = 0;

		virtual void drawIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) = 0;

		virtual void drawIndexedIndirectCount_(GPUBuffer* indirectCommandBuffer, GPUBuffer* countBuffer, uint32_t maxCount, uint32_t commandOffset, uint32_t countOffset, const std::string& label) = 0;
	};

	class RenderPipelineBuilder {
	public:
		RenderPipelineBuilder(Shader vertexShader, Shader fragmentShader);

		[[nodiscard]] const Shader& getVertexShader() const {
			return mVertexShader;
		}

		[[nodiscard]] const Shader& getFragmentShader() const {
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

		[[nodiscard]] BlendMode getBlendMode() const {
			return mBlendMode;
		}

		RenderPipelineBuilder* blendMode(BlendMode blendMode);

		[[nodiscard]] const std::vector<DescriptorLayout*>& getDescriptorLayouts() const {
			return mDescriptorLayouts;
		}

		RenderPipelineBuilder* descriptorLayout(uint32_t set, DescriptorLayout* descriptorLayout);

		virtual std::unique_ptr<RenderPipeline> build() = 0;

		virtual ~RenderPipelineBuilder() = default;

	protected:
		Shader mVertexShader;
		Shader mFragmentShader;
		PrimitiveTopology mPrimitiveTopology = PrimitiveTopology::TRIANGLE_LIST;
		PolygonMode mPolygonMode = PolygonMode::FILL;
		float mLineWidth = 1.0f;
		CullMode mCullMode = CullMode::NONE;
		FrontFace mFrontFace = FrontFace::COUNTER_CLOCKWISE;
		ImageFormat mColourFormat = ImageFormat::RGBA16_SFLOAT;
		ImageFormat mDepthFormat = ImageFormat::D32_SFLOAT;
		BlendMode mBlendMode = BlendMode::NONE;
		std::vector<DescriptorLayout*> mDescriptorLayouts;
	};
}
