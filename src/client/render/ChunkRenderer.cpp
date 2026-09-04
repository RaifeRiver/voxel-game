#include "ChunkRenderer.h"

#include "glm/packing.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "common/chunk/Chunk.h"
#include "common/ecs/ECSRegistry.h"
#include "common/util/FileHelper.h"
#include "common/util/Log.h"
#include "engine/RenderEngine.h"

namespace voxel_game::client::render {
	ChunkRenderer::ChunkRenderer(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<engine::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		const std::string vertexShader = util::readFileAsString(resourceManager.findResource("voxel_game:shaders/chunk", ".vert", resource::ResourceType::ASSET).path);
		const std::string fragmentShader = util::readFileAsString(resourceManager.findResource("voxel_game:shaders/chunk", ".frag", resource::ResourceType::ASSET).path);
		mPipeline = renderEngine.createRenderPipelineBuilder(vertexShader, fragmentShader)->depthFormat(renderEngine.getDepthImage().getFormat())->cullMode(engine::CullMode::BACK)->build();

		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->build(1, engine::ShaderStage::VERTEX);
		mDescriptorSet = mDescriptorAllocator->allocate();

		std::srand(321985123);
		chunk::Chunk chunk({0, 0, 0}, registry.createEntity());
		for (uint32_t x = 0; x < chunk::CHUNK_SIZE; x++) {
			for (uint32_t z = 0; z < chunk::CHUNK_SIZE; z++) {
				const uint32_t height = static_cast<uint32_t>((static_cast<double>(std::rand()) / RAND_MAX * 0.2 + 0.6) * chunk::CHUNK_SIZE);
				for (uint32_t y = 0; y < height; y++) {
					chunk.setBlock(x, y, z, y + 1);
				}
			}
		}

		std::vector<ChunkVertex> vertices;
		for (uint32_t x = 0; x < chunk::CHUNK_SIZE; x++) {
			for (uint32_t y = 0; y < chunk::CHUNK_SIZE; y++) {
				for (uint32_t z = 0; z < chunk::CHUNK_SIZE; z++) {
					const uint32_t block = chunk.getBlock(x, y, z);
					if (block != 0) {
						const uint32_t colour = glm::packUnorm4x8({block / 32.0f, 0, 0, 1});

						glm::vec3 p0 = {x, y, z + 1.0f};
						glm::vec3 p1 = {x + 1.0f, y, z + 1.0f};
						glm::vec3 p2 = {x + 1.0f, y + 1.0f, z + 1.0f};
						glm::vec3 p3 = {x, y + 1.0f, z + 1.0f};
						glm::vec3 p4 = {x, y, z};
						glm::vec3 p5 = {x + 1.0f, y, z};
						glm::vec3 p6 = {x + 1.0f, y + 1.0f, z};
						glm::vec3 p7 = {x, y + 1.0f, z};

						vertices.push_back({.position = p0, .colour = colour});
						vertices.push_back({.position = p1, .colour = colour});
						vertices.push_back({.position = p2, .colour = colour});
						vertices.push_back({.position = p0, .colour = colour});
						vertices.push_back({.position = p2, .colour = colour});
						vertices.push_back({.position = p3, .colour = colour});

						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p7, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p5, .colour = colour});

						vertices.push_back({.position = p3, .colour = colour});
						vertices.push_back({.position = p2, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p3, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p7, .colour = colour});

						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p5, .colour = colour});
						vertices.push_back({.position = p1, .colour = colour});
						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p1, .colour = colour});
						vertices.push_back({.position = p0, .colour = colour});

						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p0, .colour = colour});
						vertices.push_back({.position = p3, .colour = colour});
						vertices.push_back({.position = p4, .colour = colour});
						vertices.push_back({.position = p3, .colour = colour});
						vertices.push_back({.position = p7, .colour = colour});

						vertices.push_back({.position = p1, .colour = colour});
						vertices.push_back({.position = p5, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p1, .colour = colour});
						vertices.push_back({.position = p6, .colour = colour});
						vertices.push_back({.position = p2, .colour = colour});
					}
				}
			}
		}
		LOG_INFO("Vertices: {}, triangles: {}", vertices.size(), vertices.size() / 6);
		mVertexBuffer = renderEngine.allocateBuffer(vertices.size() * sizeof(ChunkVertex), engine::BufferUsage::SHADER_DEVICE_ADDRESS, engine::MemoryType::GPU, engine::MappedType::SEQUENTIAL_WRITE);
		memcpy(mVertexBuffer->map(), vertices.data(), vertices.size() * sizeof(ChunkVertex));
		mVertexBuffer->unmap();

		mPushConstants.vertexBufferAddress = mVertexBuffer->getDeviceAddress();
	}

	void ChunkRenderer::runStage(const ecs::SystemStage stage, ecs::ECSRegistry& registry, float) {
		if (stage != ecs::SystemStage::RENDER) {
			return;
		}

		auto& renderEngine = registry.getResource<engine::RenderEngine>();

		const glm::uvec2 size = renderEngine.getRenderImage().getSize();
		mPushConstants.viewProj = glm::perspective(glm::radians(90.0f), static_cast<float>(size.x) / size.y, 1000.0f, 0.1f) * glm::lookAt(glm::vec3(16, 50, 45), glm::vec3(16, 32, 16), glm::vec3(0, 1, 0));

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->setPushConstants(&mPushConstants);
		mPipeline->draw(mVertexBuffer->getSize() / sizeof(ChunkVertex));

		renderEngine.endRendering();
	}
}
