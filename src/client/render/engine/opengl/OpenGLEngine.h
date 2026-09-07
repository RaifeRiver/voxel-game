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

#include "glad/glad.h"

#include "OpenGLImage.h"
#include "client/render/engine/RenderEngine.h"
#include "client/window/Window.h"
#include "common/ecs/ECSRegistry.h"

namespace voxel_game::client::render::engine::opengl {
	class OpenGLEngine : public RenderEngine {
	public:
		explicit OpenGLEngine(ecs::ECSRegistry& registry);

		std::unique_ptr<GPUImage> allocateImage(glm::ivec3 size, ImageFormat format, ImageUsage usage, ImageType type) override;

		std::unique_ptr<ComputePipeline> createComputePipeline(const Shader& computeShader) override;

		std::unique_ptr<RenderPipelineBuilder> createRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader) override;

		std::unique_ptr<DescriptorAllocatorBuilder> createDescriptorAllocatorBuilder() override;

		[[nodiscard]] GPUImage& getRenderImage() override {
			return *mRenderImage;
		}

		[[nodiscard]] GPUImage& getDepthImage() override {
			return *mDepthImage;
		}

		void beginRendering() override;

		void endRendering() override;

		void waitForGPU() override;

		void destroy() override;

	protected:
		std::unique_ptr<GPUBuffer> allocateBuffer_(size_t size, BufferUsage usage, MemoryType memoryType, MappedType mappedType) override;

	private:
		std::unique_ptr<OpenGLImage> mRenderImage = nullptr;
		std::unique_ptr<OpenGLImage> mDepthImage = nullptr;
		GLuint mFramebufferObject = 0;
		GLsync mRenderFences[FRAME_OVERLAP] = {};

		void initOpenGL(window::Window& window);

		void preRender();

		void postRender(window::Window& window);
	};
}
