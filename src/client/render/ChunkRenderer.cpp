#include "ChunkRenderer.h"

#include "glm/packing.hpp"
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#include "common/chunk/Chunk.h"
#include "common/component/Transform.h"
#include "common/ecs/ECSRegistry.h"
#include "common/player/CameraRotation.h"
#include "common/player/Player.h"
#include "common/util/FileHelper.h"
#include "common/util/Log.h"
#include "engine/RenderEngine.h"

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

		TracyCZoneN(create_chunk, "Create test chunk", 1);
		std::srand(321985123);
		chunk::Chunk chunk({0, 0, 0}, registry.createEntity());
		for (uint32_t x = 0; x < chunk::CHUNK_SIZE; x++) {
			for (uint32_t z = 0; z < chunk::CHUNK_SIZE; z++) {
				const uint32_t height = static_cast<uint32_t>((static_cast<double>(std::rand()) / RAND_MAX * 0.2 + 0.6) * chunk::CHUNK_SIZE);
				for (uint32_t y = 0; y < height; y++) {
					chunk.setBlock(x, y, z, z * 32 * 32 + y * 32 + x + 1);
				}
			}
		}
		TracyCZoneEnd(create_chunk);

		TracyCZoneN(mesh_chunk, "Mesh chunk", 1);
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
		TracyCZoneEnd(mesh_chunk);

		TracyCZoneN(upload_chunk, "Upload chunk", 1);
		LOG_INFO("Vertices: {}, triangles: {}", vertices.size(), vertices.size() / 6);
		mVertexBuffer = renderEngine.allocateBuffer(vertices.size() * sizeof(ChunkVertex), engine::BufferUsage::SHADER_DEVICE_ADDRESS, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		memcpy(mVertexBuffer->map(), vertices.data(), vertices.size() * sizeof(ChunkVertex));
		mVertexBuffer->unmap();
		TracyCZoneEnd(upload_chunk);

		mPushConstants.vertexBufferAddress = mVertexBuffer->getDeviceAddress();
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

		const glm::uvec2 size = renderEngine.getRenderImage().getSize();
		const glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), static_cast<float>(size.x) / size.y, 1000000.0f, 0.1f);
		const glm::quat pitchRotation = glm::angleAxis(rotation.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::quat yawRotation = glm::angleAxis(rotation.yaw, glm::vec3(0.0f, -1.0f, 0.0f));
		const glm::mat4 rotationMatrix = glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
		const glm::mat4 translationMatrix  =glm::translate(glm::mat4(1.0f), glm::vec3(transform.pos.sector) * universe::SECTOR_SIZE + transform.pos.local);
		const glm::mat4 viewMatrix = glm::inverse(translationMatrix * rotationMatrix);
		mPushConstants.viewProj = projectionMatrix * viewMatrix;

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->setPushConstants(&mPushConstants);
		mPipeline->draw(mVertexBuffer->getSize() / sizeof(ChunkVertex), 0, "Render chunk");

		renderEngine.endRendering();
	}
}
