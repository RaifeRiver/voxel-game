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

#include "glm/packing.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/event/LoadChunkEvent.h"
#include "common/event/UnloadChunkEvent.h"
#include "common/player/CameraRotation.h"
#include "common/player/Player.h"
#include "common/util/FileHelper.h"
#include "common/util/Log.h"
#include "engine/IndirectCommand.h"

namespace voxel_game::client::render {
	ChunkRenderer::ChunkRenderer(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init chunk renderer");

		engine::RenderEngine& renderEngine = registry.getResource<engine::RenderEngine>();
		const resource::ResourceManager& resourceManager = registry.getResource<resource::ResourceManager>();

		mDescriptorLayout = renderEngine.createDescriptorLayoutBuilder()->addBinding(0, engine::DescriptorType::STORAGE_BUFFER)->addBinding(1, engine::DescriptorType::STORAGE_BUFFER)->addBinding(2, engine::DescriptorType::STORAGE_BUFFER)->build();

		const engine::Shader cullingShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/culling", ".comp", resource::ResourceType::ASSET).path).build(engine::ShaderStage::COMPUTE);
		mCullingPipeline = renderEngine.createComputePipelineBuilder(cullingShader)->descriptorLayout(0, mDescriptorLayout.get())->build();

		const engine::Shader vertexShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/main", ".vert", resource::ResourceType::ASSET).path).build(engine::ShaderStage::VERTEX);
		const engine::Shader fragmentShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk/main", ".frag", resource::ResourceType::ASSET).path).build(engine::ShaderStage::FRAGMENT);
		mRenderPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->depthFormat(renderEngine.getDepthImage().getFormat())->cullMode(engine::CullMode::BACK)->descriptorLayout(0, mDescriptorLayout.get())->build();

		mChunkBuffer = renderEngine.allocateBuffer(sizeof(chunk::Chunk) * 1100000, engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST | engine::BufferUsage::TRANSFER_SRC, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		mIndirectCommandBuffer = renderEngine.allocateBuffer(sizeof(engine::IndirectCommand) * 1100000, engine::BufferUsage::STORAGE | engine::BufferUsage::INDIRECT, engine::MemoryType::GPU);
		mCountBuffer = renderEngine.allocateBuffer(sizeof(uint32_t), engine::BufferUsage::STORAGE | engine::BufferUsage::TRANSFER_DST | engine::BufferUsage::INDIRECT, engine::MemoryType::GPU);

		mDescriptorAllocator = mDescriptorLayout->createAllocator(1);
		mDescriptorSet = mDescriptorAllocator->allocate();
		mDescriptorSet->setBinding(0, mChunkBuffer.get());
		mDescriptorSet->setBinding(1, mIndirectCommandBuffer.get());
		mDescriptorSet->setBinding(2, mCountBuffer.get());
	}

	void ChunkRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::RENDER) {
			return;
		}

		ZoneScopedN("Render chunks");

		auto& renderEngine = registry.getResource<engine::RenderEngine>();

		for (const event::LoadChunkEvent* event: registry.getEvents<event::LoadChunkEvent>()) {
			if (!registry.hasComponent<ChunkMeshData>(event->entity)) {
				continue;
			}
			ChunkMeshData& meshData = registry.getComponent<ChunkMeshData>(event->entity);
			ChunkMesh mesh = meshChunk(renderEngine, event->chunk);
			if (mesh.hasMesh) {
				meshData.meshes[event->chunk.getPos()] = std::move(mesh);
			}
		}
		for (const event::UnloadChunkEvent* event: registry.getEvents<event::UnloadChunkEvent>()) {
			if (!registry.hasComponent<ChunkMeshData>(event->entity)) {
				continue;
			}
			ChunkMeshData& meshData = registry.getComponent<ChunkMeshData>(event->entity);
			auto it = meshData.meshes.find(event->chunk);
			if (it != meshData.meshes.end()) {
				if (mNextChunk != 0) {
					mChunkBuffer->copyFromBuffer(*mChunkBuffer, --mNextChunk * sizeof(Chunk), it->second.chunkIndex * sizeof(Chunk), sizeof(Chunk));
					mChunkBuffer->barrier(engine::BufferAccess::TRANSFER_WRITE, engine::BufferAccess::SHADER_READ, it->second.chunkIndex * sizeof(Chunk), sizeof(Chunk));
				}
				meshData.meshes.erase(event->chunk);
			}
		}

		if (mNextChunk == 0) {
			return;
		}

		const ecs::Entity player = registry.getEntitiesWithComponents<player::LocalPlayer, component::Transform>()[0];
		const component::Transform& transform = registry.getComponent<component::Transform>(player);
		const player::CameraRotation& rotation = registry.getComponent<player::CameraRotation>(player);

		const glm::uvec2 size = renderEngine.getRenderImage().getSize();
		const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(size.x) / size.y, 1000000.0f, 0.1f);
		const glm::quat pitchRotation = glm::angleAxis(rotation.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::quat yawRotation = glm::angleAxis(rotation.yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		const glm::mat4 rotationMatrix = glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
		const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.pos.sector) * universe::SECTOR_SIZE + transform.pos.local);
		const glm::mat4 viewMatrix = glm::inverse(translationMatrix * rotationMatrix);
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

		mCullingPipeline->dispatch((mNextChunk + 63) >> 6);

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

	ChunkMesh ChunkRenderer::meshChunk(engine::RenderEngine& renderEngine, const chunk::Chunk& chunk) {
		ZoneScopedN("Mesh chunk");

		ChunkMesh mesh;
		if (!chunk.isUniform() || chunk.getBlock(0, 0, 0) != 0) {
			std::vector<Vertex> vertices;
			for (uint32_t x = 0; x < chunk::CHUNK_SIZE; x++) {
				for (uint32_t y = 0; y < chunk::CHUNK_SIZE; y++) {
					for (uint32_t z = 0; z < chunk::CHUNK_SIZE; z++) {
						const uint32_t block = chunk.getBlock(x, y, z);
						if (block != 0) {
							const uint32_t colour = glm::packUnorm4x8({(block - 1) % 1024 / 32 / 31.0f, (block - 1) % 32 / 31.0f, (block - 1) / 1024 / 31.0f, 1});

							if (z == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y, z + 1) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 0, .colour = colour});
							}

							if (z == 0 || chunk.getBlock(x, y, z - 1) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 1, .colour = colour});
							}

							if (y == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y + 1, z) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 2, .colour = colour});
							}

							if (y == 0 || chunk.getBlock(x, y - 1, z) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 3, .colour = colour});
							}

							if (x == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x + 1, y, z) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 4, .colour = colour});
							}

							if (x == 0 || chunk.getBlock(x - 1, y, z) == 0) {
								vertices.push_back({.pos = {x, y, z}, .dir = 5, .colour = colour});
							}
						}
					}
				}
			}

			if (vertices.size() > 0) {
				mesh.chunkIndex = mNextChunk++;

				std::unique_ptr<engine::GPUBuffer> buffer = renderEngine.allocateBuffer(vertices.size() * sizeof(Vertex), engine::BufferUsage::SHADER_DEVICE_ADDRESS, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
				memcpy(buffer->map(), vertices.data(), vertices.size() * sizeof(Vertex));
				buffer->unmap();
				mesh.vertexBuffer = std::move(buffer);

				auto chunkBufferData = static_cast<Chunk*>(mChunkBuffer->map());
				const glm::ivec3 pos = chunk.getPos();
				auto& [modelMatrix, boundingSphere, bufferAddress, vertexCount] = chunkBufferData[mesh.chunkIndex];
				modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pos) * chunk::CHUNK_SIZE);
				boundingSphere = glm::ivec4(pos * static_cast<int32_t>(chunk::CHUNK_SIZE) + static_cast<int32_t>(chunk::CHUNK_SIZE) / 2, std::ceil(std::sqrt(static_cast<float>(chunk::CHUNK_SIZE * chunk::CHUNK_SIZE / 2 * 3))));
				bufferAddress = mesh.vertexBuffer->getDeviceAddress();
				vertexCount = vertices.size() * 6;
				mChunkBuffer->unmap();

				mesh.hasMesh = true;
			}
		}

		return mesh;
	}
}
