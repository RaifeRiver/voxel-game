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

namespace voxel_game::client::render {
	ChunkRenderer::ChunkRenderer(ecs::ECSRegistry& registry) {
		ZoneScopedN("Init chunk renderer");

		auto& renderEngine = registry.getResource<engine::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		const engine::Shader vertexShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk", ".vert", resource::ResourceType::ASSET).path).build(engine::ShaderStage::VERTEX);
		const engine::Shader fragmentShader = engine::ShaderBuilder(resourceManager.findResource("voxel_game:shaders/chunk", ".frag", resource::ResourceType::ASSET).path).build(engine::ShaderStage::FRAGMENT);
		mPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->depthFormat(renderEngine.getDepthImage().getFormat())->cullMode(engine::CullMode::BACK)->build();

		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->build(1, engine::ShaderStage::VERTEX);
		mDescriptorSet = mDescriptorAllocator->allocate();
	}

	void ChunkRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::RENDER) {
			return;
		}

		ZoneScopedN("Render chunk");

		auto& renderEngine = registry.getResource<engine::RenderEngine>();

		const ecs::Entity player = registry.getEntitiesWithComponents<player::LocalPlayer, component::Transform>()[0];
		const component::Transform& transform = registry.getComponent<component::Transform>(player);
		const player::CameraRotation& rotation = registry.getComponent<player::CameraRotation>(player);

		for (const event::LoadChunkEvent* event: registry.getEvents<event::LoadChunkEvent>()) {
			if (!registry.hasComponent<ChunkMeshData>(event->entity)) {
				continue;
			}
			ChunkMeshData& meshData = registry.getComponent<ChunkMeshData>(event->entity);
			ChunkMesh mesh = meshChunk(renderEngine, event->chunk);
			if (mesh.vertexBuffer) {
				meshData.meshes[event->chunk.getPos()] = std::move(mesh);
			}
		}
		for (const event::UnloadChunkEvent* event: registry.getEvents<event::UnloadChunkEvent>()) {
			if (!registry.hasComponent<ChunkMeshData>(event->entity)) {
				continue;
			}
			ChunkMeshData& meshData = registry.getComponent<ChunkMeshData>(event->entity);
			meshData.meshes.erase(event->chunk);
		}

		const glm::uvec2 size = renderEngine.getRenderImage().getSize();
		const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(size.x) / size.y, 1000000.0f, 0.1f);
		const glm::quat pitchRotation = glm::angleAxis(rotation.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::quat yawRotation = glm::angleAxis(rotation.yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		const glm::mat4 rotationMatrix = glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
		const glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.pos.sector) * universe::SECTOR_SIZE + transform.pos.local);
		const glm::mat4 viewMatrix = glm::inverse(translationMatrix * rotationMatrix);
		const glm::mat4 viewProj = projectionMatrix * viewMatrix;

		renderEngine.beginRendering();

		mPipeline->bind();

		ChunkPushConstants pushConstants = {};
		for (const ecs::Entity entity : registry.getEntitiesWithComponents<ChunkMeshData>()) {
			const ChunkMeshData& meshData = registry.getComponent<ChunkMeshData>(entity);
			for (auto& [pos, mesh] : meshData.meshes) {
				pushConstants.viewProj = viewProj * glm::translate(glm::mat4(1.0f), glm::vec3(pos) * chunk::CHUNK_SIZE);
				pushConstants.vertexBufferAddress = mesh.vertexBuffer->getDeviceAddress();
				mPipeline->setPushConstants(&pushConstants);
				mPipeline->draw(mesh.vertexBuffer->getSize() / sizeof(ChunkVertex), 0, "Render chunk");
			}
		}

		renderEngine.endRendering();
	}

	ChunkMesh ChunkRenderer::meshChunk(engine::RenderEngine& renderEngine, const chunk::Chunk& chunk) {
		ZoneScopedN("Mesh chunk");

		ChunkMesh mesh;
		if (!chunk.isUniform() || chunk.getBlock(0, 0, 0) != 0) {
			std::vector<ChunkVertex> vertices;
			for (uint32_t x = 0; x < chunk::CHUNK_SIZE; x++) {
				for (uint32_t y = 0; y < chunk::CHUNK_SIZE; y++) {
					for (uint32_t z = 0; z < chunk::CHUNK_SIZE; z++) {
						const uint32_t block = chunk.getBlock(x, y, z);
						if (block != 0) {
							const uint32_t colour = glm::packUnorm4x8({(block - 1) % 1024 / 32 / 31.0f, (block - 1) % 32 / 31.0f, (block - 1) / 1024 / 31.0f, 1});

							glm::vec3 p0 = {x, y, z + 1.0f};
							glm::vec3 p1 = {x + 1.0f, y, z + 1.0f};
							glm::vec3 p2 = {x + 1.0f, y + 1.0f, z + 1.0f};
							glm::vec3 p3 = {x, y + 1.0f, z + 1.0f};
							glm::vec3 p4 = {x, y, z};
							glm::vec3 p5 = {x + 1.0f, y, z};
							glm::vec3 p6 = {x + 1.0f, y + 1.0f, z};
							glm::vec3 p7 = {x, y + 1.0f, z};

							if (z == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y, z + 1) == 0) {
								vertices.push_back({.position = p0, .colour = colour});
								vertices.push_back({.position = p1, .colour = colour});
								vertices.push_back({.position = p2, .colour = colour});
								vertices.push_back({.position = p0, .colour = colour});
								vertices.push_back({.position = p2, .colour = colour});
								vertices.push_back({.position = p3, .colour = colour});
							}

							if (z == 0 || chunk.getBlock(x, y, z - 1) == 0) {
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p7, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p5, .colour = colour});
							}

							if (y == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x, y + 1, z) == 0) {
								vertices.push_back({.position = p3, .colour = colour});
								vertices.push_back({.position = p2, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p3, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p7, .colour = colour});
							}

							if (y == 0 || chunk.getBlock(x, y - 1, z) == 0) {
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p5, .colour = colour});
								vertices.push_back({.position = p1, .colour = colour});
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p1, .colour = colour});
								vertices.push_back({.position = p0, .colour = colour});
							}

							if (x == chunk::CHUNK_SIZE - 1 || chunk.getBlock(x + 1, y, z) == 0) {
								vertices.push_back({.position = p1, .colour = colour});
								vertices.push_back({.position = p5, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p1, .colour = colour});
								vertices.push_back({.position = p6, .colour = colour});
								vertices.push_back({.position = p2, .colour = colour});
							}

							if (x == 0 || chunk.getBlock(x - 1, y, z) == 0) {
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p0, .colour = colour});
								vertices.push_back({.position = p3, .colour = colour});
								vertices.push_back({.position = p4, .colour = colour});
								vertices.push_back({.position = p3, .colour = colour});
								vertices.push_back({.position = p7, .colour = colour});
							}
						}
					}
				}
			}

			if (vertices.size() > 0) {
				std::unique_ptr<engine::GPUBuffer> buffer = renderEngine.allocateBuffer(vertices.size() * sizeof(ChunkVertex), engine::BufferUsage::SHADER_DEVICE_ADDRESS, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
				memcpy(buffer->map(), vertices.data(), vertices.size() * sizeof(ChunkVertex));
				buffer->unmap();

				mesh.vertexBuffer = std::move(buffer);
			}
		}

		return mesh;
	}
}
