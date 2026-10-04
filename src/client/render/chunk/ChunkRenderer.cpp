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

#include "ChunkRenderer.h"

#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#include "client/render/engine/IndirectCommand.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadChunkEvent.h"
#include "common/event/LoadPlanetEvent.h"
#include "common/event/UnloadChunkEvent.h"
#include "common/player/CameraRotation.h"
#include "common/player/Player.h"
#include "common/util/FileHelper.h"
#include "common/util/Log.h"

namespace voxel_game::client::render::chunk {
	ChunkRenderer::ChunkRenderer(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init chunk renderer");

		engine::RenderEngine& renderEngine = registry.getResource<engine::RenderEngine>();
		const resource::ResourceManager& resourceManager = registry.getResource<resource::ResourceManager>();

		mUseBufferReference = renderEngine.getSupportedFeatures().shaderFeatures.bufferReference;

		const std::unique_ptr<engine::DescriptorLayoutBuilder> descriptorLayoutBuilder = renderEngine.createDescriptorLayoutBuilder();
		descriptorLayoutBuilder->addBinding(0, engine::DescriptorType::STORAGE_BUFFER)->addBinding(1, engine::DescriptorType::STORAGE_BUFFER)->addBinding(2, engine::DescriptorType::STORAGE_BUFFER)->addBinding(3, engine::DescriptorType::STORAGE_BUFFER);
		if (!mUseBufferReference) {
			descriptorLayoutBuilder->addBinding(4, engine::DescriptorType::STORAGE_BUFFER);
		}
		mDescriptorLayout = descriptorLayoutBuilder->build();

		const engine::Shader cullingShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/culling", ".comp", resource::ResourceType::ASSET).path).build(engine::ShaderStage::COMPUTE);
		mCullingPipeline = renderEngine.createComputePipelineBuilder(cullingShader)->descriptorLayout(0, mDescriptorLayout.get())->build();

		const engine::Shader vertexShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/main", ".vert", resource::ResourceType::ASSET).path).build(engine::ShaderStage::VERTEX);
		const engine::Shader fragmentShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/main", ".frag", resource::ResourceType::ASSET).path).build(engine::ShaderStage::FRAGMENT);
		mRenderPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->depthFormat(renderEngine.getDepthImage().getFormat())->cullMode(engine::CullMode::BACK)->descriptorLayout(0, mDescriptorLayout.get())->build();

		mChunkBuffer = renderEngine.allocateBuffer(sizeof(chunk::Chunk) * 1100000, engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST | engine::BufferUsage::TRANSFER_SRC, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		for (std::unique_ptr<engine::GPUBuffer>& frameChunkBuffer : mFrameChunkBuffers) {
			frameChunkBuffer = renderEngine.allocateBuffer(sizeof(FrameChunk) * 1100000, engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST | engine::BufferUsage::TRANSFER_SRC, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		}
		mIndirectCommandBuffer = renderEngine.allocateBuffer(sizeof(engine::IndirectCommand) * 1100000, engine::BufferUsage::STORAGE | engine::BufferUsage::INDIRECT, engine::MemoryType::GPU);
		mCountBuffer = renderEngine.allocateBuffer(sizeof(uint32_t), engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST | engine::BufferUsage::INDIRECT, engine::MemoryType::GPU);
		if (!mUseBufferReference) {
			mFaceBuffer = renderEngine.allocateBuffer(sizeof(Face) * 16384 * 10000, engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		}

		mDescriptorAllocator = mDescriptorLayout->createAllocator(1);
		mDescriptorSet = mDescriptorAllocator->allocate();
		mDescriptorSet->setBinding(0, mChunkBuffer.get());
		mDescriptorSet->setBinding(1, mIndirectCommandBuffer.get());
		mDescriptorSet->setBinding(2, mCountBuffer.get());
		if (!mUseBufferReference) {
			mDescriptorSet->setBinding(4, mFaceBuffer.get());
		}
	}

	void ChunkRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::RENDER) {
			return;
		}

		ZoneScopedN("Render chunks");

		engine::RenderEngine& renderEngine = registry.getResource<engine::RenderEngine>();

		for (const event::LoadPlanetEvent* event: registry.getEvents<event::LoadPlanetEvent>()) {
			registry.attachComponent<ChunkRenderData>(event->planet);
		}

		for (const event::LoadChunkEvent* event: registry.getEvents<event::LoadChunkEvent>()) {
			if (!registry.hasComponent<ChunkRenderData>(event->entity)) {
				continue;
			}
			ChunkRenderData& renderData = registry.getComponent<ChunkRenderData>(event->entity);
			auto [faces] = meshChunk(event->chunk);
			if (!faces.empty()) {
				ChunkRender render = {
					.chunkIndex = mNextChunk++
				};

				const auto chunkBufferData = static_cast<Chunk*>(mChunkBuffer->map());
				Chunk chunkData = chunkBufferData[render.chunkIndex];
				chunkData.vertexCount = faces.size() * 6;
				if (mUseBufferReference) {
					std::unique_ptr<engine::GPUBuffer> buffer = renderEngine.allocateBuffer(faces.size() * sizeof(Face), engine::BufferUsage::SHADER_DEVICE_ADDRESS, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
					memcpy(buffer->map(), faces.data(), faces.size() * sizeof(Face));
					buffer->unmap();
					render.vertexBuffer = std::move(buffer);
					chunkData.faceBuffer = render.vertexBuffer->getDeviceAddress();
				}
				else {
					chunkData.faceOffset = mFaceBufferPointer;
				}
				mChunkBuffer->unmap();
				if (!mUseBufferReference) {
					memcpy(static_cast<Face*>(mFaceBuffer->map()) + mFaceBufferPointer, faces.data(), faces.size() * sizeof(Face));
					mFaceBuffer->unmap();
					mFaceBufferPointer += faces.size();;
				}

				renderData.data[event->chunk.getPos()] = std::move(render);
				uint32_t index = render.chunkIndex;
				if (renderData.positions.size() <= index) {
					renderData.positions.resize(index + 1);
				}
				renderData.positions[index] = event->chunk.getPos();
			}
		}
		for (const event::UnloadChunkEvent* event: registry.getEvents<event::UnloadChunkEvent>()) {
			if (!registry.hasComponent<ChunkRenderData>(event->entity)) {
				continue;
			}
			ChunkRenderData& renderData = registry.getComponent<ChunkRenderData>(event->entity);
			auto it = renderData.data.find(event->chunk);
			if (it != renderData.data.end()) {
				if (mNextChunk != 0) {
					mChunkBuffer->copyFromBuffer(*mChunkBuffer, (mNextChunk - 1) * sizeof(Chunk), it->second.chunkIndex * sizeof(Chunk), sizeof(Chunk));
					mChunkBuffer->barrier(engine::BufferAccess::TRANSFER_WRITE, engine::BufferAccess::SHADER_READ, it->second.chunkIndex * sizeof(Chunk), sizeof(Chunk));
					for (const std::unique_ptr<engine::GPUBuffer>& frameChunkBuffer : mFrameChunkBuffers) {
						frameChunkBuffer->copyFromBuffer(*mChunkBuffer, (mNextChunk - 1) * sizeof(FrameChunk), it->second.chunkIndex * sizeof(FrameChunk), sizeof(FrameChunk));
						frameChunkBuffer->barrier(engine::BufferAccess::TRANSFER_WRITE, engine::BufferAccess::SHADER_READ, it->second.chunkIndex * sizeof(FrameChunk), sizeof(FrameChunk));
					}
					glm::ivec3 position = renderData.positions[mNextChunk - 1];
					renderData.positions.erase(renderData.positions.begin() + (mNextChunk - 1));
					renderData.positions[it->second.chunkIndex] = position;
					renderData.data[position].chunkIndex = it->second.chunkIndex;
					mNextChunk--;
				}
				renderData.data.erase(event->chunk);
			}
		}

		if (mNextChunk == 0) {
			return;
		}

		const ecs::Entity player = registry.getEntitiesWithComponents<player::LocalPlayer, component::Transform>()[0];
		const component::Transform& transform = registry.getComponent<component::Transform>(player);
		const player::CameraRotation& rotation = registry.getComponent<player::CameraRotation>(player);

		const std::unique_ptr<engine::GPUBuffer>& frameChunkBuffer = mFrameChunkBuffers[renderEngine.getFrame()];
		auto frameChunkBufferData = static_cast<FrameChunk*>(frameChunkBuffer->map());
		for (const ecs::Entity entity : registry.getEntitiesWithComponents<ChunkRenderData, component::Transform>()) {
			const component::Transform& chunkTransform = registry.getComponent<component::Transform>(entity);
			const int32_t boundingSphereRadius = std::ceil(voxel_game::chunk::CHUNK_SIZE * glm::compMax(chunkTransform.scale) / 2.0f * std::sqrt(3.0f));
			for (auto& [pos, mesh] : registry.getComponent<ChunkRenderData>(entity).data) {
				auto& [modelMatrix, boundingSphere] = frameChunkBufferData[mesh.chunkIndex];
				auto [relativeSector, relativeLocal] = chunkTransform.pos + glm::i64vec3(pos) * static_cast<int64_t>(voxel_game::chunk::CHUNK_SIZE) - transform.pos;
				auto relativePos = glm::vec3(relativeSector * static_cast<int64_t>(universe::SECTOR_SIZE)) + relativeLocal;
				modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), relativePos), chunkTransform.scale);
				boundingSphere = glm::ivec4(relativePos + voxel_game::chunk::CHUNK_SIZE / 2.0f, boundingSphereRadius);
			}
		}
		frameChunkBuffer->unmap();
		frameChunkBuffer->barrier(engine::BufferAccess::TRANSFER_WRITE, engine::BufferAccess::SHADER_READ);

		mDescriptorSet->setBinding(3, frameChunkBuffer.get());

		const glm::uvec2 size = renderEngine.getRenderImage().getSize();
		const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(size.x) / size.y, 1000000.0f, 0.1f);
		const glm::quat pitchRotation = glm::angleAxis(rotation.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::quat yawRotation = glm::angleAxis(rotation.yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		const glm::mat4 rotationMatrix = glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
		const glm::mat4 viewMatrix = glm::inverse(rotationMatrix);
		const glm::mat4 viewProj = projectionMatrix * viewMatrix;

		mCountBuffer->fill(0, sizeof(uint32_t), 0);
		mCountBuffer->barrier(engine::BufferAccess::TRANSFER_WRITE, engine::BufferAccess::SHADER_WRITE);

		mCullingPipeline->bind();
		mCullingPipeline->bindDescriptorSet(0, mDescriptorSet.get());

		const glm::mat4 m = glm::transpose(viewProj);
		CullingPushConstants cullingPushConstants = {};
		cullingPushConstants.frustumPlanes[0] = m[3] + m[0];
		cullingPushConstants.frustumPlanes[1] = m[3] - m[0];
		cullingPushConstants.frustumPlanes[2] = m[3] + m[1];
		cullingPushConstants.frustumPlanes[3] = m[3] - m[1];
		cullingPushConstants.frustumPlanes[4] = m[2];
		cullingPushConstants.frustumPlanes[5] = m[3] - m[2];
		for (glm::vec4& frustumPlane: cullingPushConstants.frustumPlanes) {
			frustumPlane /= glm::length(glm::vec3(frustumPlane));
		}
		cullingPushConstants.chunkCount = mNextChunk;
		mCullingPipeline->setPushConstants(&cullingPushConstants);

		mCullingPipeline->dispatch((mNextChunk + 63) >> 6, 1, 1, "Chunk culling");

		mCountBuffer->barrier(engine::BufferAccess::SHADER_WRITE, engine::BufferAccess::INDIRECT_READ);
		mIndirectCommandBuffer->barrier(engine::BufferAccess::SHADER_WRITE, engine::BufferAccess::INDIRECT_READ);

		renderEngine.beginRendering();

		mRenderPipeline->bind();
		mRenderPipeline->bindDescriptorSet(0, mDescriptorSet.get());

		PushConstants pushConstants = {};
		pushConstants.viewProj = viewProj;
		mRenderPipeline->setPushConstants(&pushConstants);

		mRenderPipeline->drawIndirectCount(mIndirectCommandBuffer.get(), mCountBuffer.get(), mNextChunk, "Render chunk");

		renderEngine.endRendering();
	}
}
