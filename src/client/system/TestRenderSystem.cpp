#include "TestRenderSystem.h"

#include "common/ecs/ECSRegistry.h"
#include "common/resource/ResourceManager.h"

namespace voxel_game::client::system {
	TestRenderSystem::TestRenderSystem(ecs::ECSRegistry& registry) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();
		const auto& resourceManager = registry.getResource<resource::ResourceManager>();

		mPipeline = renderEngine.createRenderPipelineBuilder(resourceManager.findResource("voxel_game:shaders/test", ".vert.spv", resource::ResourceType::ASSET).path, resourceManager.findResource("voxel_game:shaders/test", ".frag.spv", resource::ResourceType::ASSET).path)->build();

		mDescriptorAllocator = renderEngine.createDescriptorAllocatorBuilder()->addBinding(0, render::DescriptorType::STORAGE_BUFFER)->build(render::FRAME_OVERLAP, render::ShaderStage::VERTEX);

		for (std::unique_ptr<render::DescriptorSet>& descriptorSet : mDescriptorSets) {
			descriptorSet = mDescriptorAllocator->allocate();
		}

		mVertexBuffer = renderEngine.allocateBuffer(8 * sizeof(glm::vec4), render::BufferUsage::STORAGE, render::MemoryType::AUTO, render::MappedType::SEQUENTIAL_WRITE);
		auto* vertexData = static_cast<glm::vec4*>(mVertexBuffer->map());
		vertexData[0] = {0.5f, -0.5f, 0.0f, 1.0f};
		vertexData[1] = {1.0f, 0.0f, 0.0f, 1.0f};
		vertexData[2] = {0.5f, 0.5f, 0.0f, 1.0f};
		vertexData[3] = {0.0f, 1.0f, 0.0f, 1.0f};
		vertexData[4] = {-0.5f, -0.5f, 0.0f, 1.0f};
		vertexData[5] = {0.0f, 0.0f, 1.0f, 1.0f};
		vertexData[6] = {-0.5f, 0.5f, 0.0f, 1.0f};
		vertexData[7] = {0.0f, 0.0f, 0.0f, 1.0f};
		mVertexBuffer->unmap();

		mIndexBuffer = renderEngine.allocateBuffer(6 * sizeof(uint32_t), render::BufferUsage::INDEX, render::MemoryType::AUTO, render::MappedType::SEQUENTIAL_WRITE);
		auto* indexData = static_cast<uint32_t*>(mIndexBuffer->map());
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;
		indexData[3] = 2;
		indexData[4] = 1;
		indexData[5] = 3;
		mIndexBuffer->unmap();
	}

	void TestRenderSystem::render(ecs::ECSRegistry& registry, float) {
		auto& renderEngine = registry.getResource<render::RenderEngine>();

		render::DescriptorSet* descriptorSet = mDescriptorSets[renderEngine.getFrame() % render::FRAME_OVERLAP].get();
		descriptorSet->setBinding(0, mVertexBuffer.get());

		renderEngine.beginRendering();

		mPipeline->bind();
		mPipeline->bindDescriptorSet(0, descriptorSet);
		mPipeline->bindIndexBuffer(mIndexBuffer.get());
		mPipeline->drawIndexed(6);

		renderEngine.endRendering();
	}
}
