#include "VulkanRenderPipeline.h"

#include "VulkanBuffer.h"
#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanUtil.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render::vulkan {
	VkPrimitiveTopology toVKPrimitiveTopology(const PrimitiveTopology topology) {
		switch (topology) {
			case PrimitiveTopology::POINT_LIST:
				return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case PrimitiveTopology::LINE_LIST:
				return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case PrimitiveTopology::LINE_STRIP:
				return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case PrimitiveTopology::TRIANGLE_LIST:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case PrimitiveTopology::TRIANGLE_STRIP:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case PrimitiveTopology::TRIANGLE_FAN:
				return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			default:
				throw std::runtime_error("Unsupported primitive topology");
		}
	}

	VkPolygonMode toVKPolygonMode(const PolygonMode mode) {
		switch (mode) {
			case PolygonMode::FILL:
				return VK_POLYGON_MODE_FILL;
			case PolygonMode::LINE:
				return VK_POLYGON_MODE_LINE;
			case PolygonMode::POINT:
				return VK_POLYGON_MODE_POINT;
			default:
				throw std::runtime_error("Unsupported polygon mode");
		}
	}

	VkCullModeFlags toVKCullMode(const CullMode mode) {
		switch (mode) {
			case CullMode::NONE:
				return VK_CULL_MODE_NONE;
			case CullMode::FRONT:
				return VK_CULL_MODE_FRONT_BIT;
			case CullMode::BACK:
				return VK_CULL_MODE_BACK_BIT;
			case CullMode::FRONT_AND_BACK:
				return VK_CULL_MODE_FRONT_AND_BACK;
			default:
				throw std::runtime_error("Unsupported cull mode");
		}
	}

	VkFrontFace toVKFrontFace(const FrontFace face) {
		switch (face) {
			case FrontFace::COUNTER_CLOCKWISE:
				return VK_FRONT_FACE_COUNTER_CLOCKWISE;
			case FrontFace::CLOCKWISE:
				return VK_FRONT_FACE_CLOCKWISE;
			default:
				throw std::runtime_error("Unsupported front face");
		}
	}

	VulkanRenderPipeline::VulkanRenderPipeline(VulkanEngine* vulkanEngine, const RenderPipelineBuilder* builder) : mVulkanEngine(vulkanEngine) {
		std::vector<uint32_t> vertexShaderData = util::readFile<uint32_t>(builder->getVertexShader());
		const VkShaderModuleCreateInfo vertexShaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = vertexShaderData.size() * sizeof(uint32_t),
			.pCode = vertexShaderData.data()
		};
		VkShaderModule vertexShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(mVulkanEngine->getDevice(), &vertexShaderModuleCreateInfo, nullptr, &vertexShaderModule));

		std::vector<uint32_t> fragmentShaderData = util::readFile<uint32_t>(builder->getFragmentShader());
		const VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = fragmentShaderData.size() * sizeof(uint32_t),
			.pCode = fragmentShaderData.data()
		};
		VkShaderModule fragmentShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(mVulkanEngine->getDevice(), &fragmentShaderModuleCreateInfo, nullptr, &fragmentShaderModule));

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfos[2] = {};
		pipelineShaderStageCreateInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		pipelineShaderStageCreateInfos[0].module = vertexShaderModule;
		pipelineShaderStageCreateInfos[0].pName = "main";
		pipelineShaderStageCreateInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		pipelineShaderStageCreateInfos[1].module = fragmentShaderModule;
		pipelineShaderStageCreateInfos[1].pName = "main";

		std::vector<uint32_t> shaderData[] = {vertexShaderData, fragmentShaderData};

		mDescriptorSetLayouts = vulkan_util::createDescriptorSetLayouts(mVulkanEngine, 2, shaderData);

		std::vector<VkPushConstantRange> pushConstantRanges = vulkan_util::getPushConstantRanges(1, shaderData);
		for (const VkPushConstantRange& pushConstantRange : pushConstantRanges) {
			mPushConstantsSize = std::max(mPushConstantsSize, pushConstantRange.offset + pushConstantRange.size);
		}
		const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size()),
			.pSetLayouts = mDescriptorSetLayouts.data(),
			.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
			.pPushConstantRanges = pushConstantRanges.data()
		};
		vulkan_util::vkCheck(vkCreatePipelineLayout(mVulkanEngine->getDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout));

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO
		};

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = toVKPrimitiveTopology(builder->getPrimitiveTopology())
		};

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.scissorCount = 1
		};

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.polygonMode = toVKPolygonMode(builder->getPolygonMode()),
			.cullMode = toVKCullMode(builder->getCullMode()),
			.frontFace = toVKFrontFace(builder->getFrontFace()),
			.lineWidth = builder->getLineWidth()
		};

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.minSampleShading = 1.0f
		};

		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.maxDepthBounds = 1.0f
		};
		if (builder->getDepthFormat() != ImageFormat::UNKNOWN) {
			pipelineDepthStencilStateCreateInfo.depthTestEnable = true;
			pipelineDepthStencilStateCreateInfo.depthWriteEnable = true;
			pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
		}

		VkPipelineColorBlendAttachmentState pipelineColourBlendAttachmentState = {
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		BlendMode blendMode = builder->getBlendMode();
		if (blendMode == BlendMode::ADDITIVE) {
			pipelineColourBlendAttachmentState.blendEnable = true;
			pipelineColourBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			pipelineColourBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineColourBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		}
		else if (blendMode == BlendMode::ALPHA) {
			pipelineColourBlendAttachmentState.blendEnable = true;
			pipelineColourBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			pipelineColourBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineColourBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		}
		else if (blendMode != BlendMode::NONE) {
			throw std::runtime_error("Unsupported blend mode");
		}

		VkPipelineColorBlendStateCreateInfo pipelineColourBlendStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = false,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &pipelineColourBlendAttachmentState
		};

		VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamicStates
		};

		VkFormat colourAttachmentFormat = toVKImageFormat(builder->getColourFormat());
		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
			.colorAttachmentCount = 1,
			.pColorAttachmentFormats = &colourAttachmentFormat,
			.depthAttachmentFormat = toVKImageFormat(builder->getDepthFormat()),
		};

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = &pipelineRenderingCreateInfo,
			.stageCount = 2,
			.pStages = pipelineShaderStageCreateInfos,
			.pVertexInputState = &pipelineVertexInputStateCreateInfo,
			.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfo,
			.pViewportState = &pipelineViewportStateCreateInfo,
			.pRasterizationState = &pipelineRasterizationStateCreateInfo,
			.pMultisampleState = &pipelineMultisampleStateCreateInfo,
			.pDepthStencilState = &pipelineDepthStencilStateCreateInfo,
			.pColorBlendState = &pipelineColourBlendStateCreateInfo,
			.pDynamicState = &pipelineDynamicStateCreateInfo,
			.layout = mPipelineLayout,
		};

		vulkan_util::vkCheck(vkCreateGraphicsPipelines(mVulkanEngine->getDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline));

		vkDestroyShaderModule(mVulkanEngine->getDevice(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(mVulkanEngine->getDevice(), fragmentShaderModule, nullptr);
	}

	void VulkanRenderPipeline::bind() {
		// ReSharper disable once CppLocalVariableMayBeConst
		VkCommandBuffer commandBuffer = mVulkanEngine->getCommandBuffer();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

		const glm::uvec3 renderImageSize = mVulkanEngine->getRenderImage().getSize();

		const VkViewport viewport = {
			.width = static_cast<float>(renderImageSize.x),
			.height = static_cast<float>(renderImageSize.y),
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(mVulkanEngine->getCommandBuffer(), 0, 1, &viewport);

		const VkRect2D scissor = {
			.extent = {
				.width = renderImageSize.x,
				.height = renderImageSize.y
			}
		};
		vkCmdSetScissor(mVulkanEngine->getCommandBuffer(), 0, 1, &scissor);
	}

	void VulkanRenderPipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		// ReSharper disable once CppLocalVariableMayBeConst
		VkDescriptorSet vulkanDescriptorSet = dynamic_cast<VulkanDescriptorSet*>(descriptorSet)->getDescriptorSet();
		vkCmdBindDescriptorSets(mVulkanEngine->getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, set, 1, &vulkanDescriptorSet, 0, nullptr);
	}

	void VulkanRenderPipeline::setPushConstants(void* pushConstants) {
		vkCmdPushConstants(mVulkanEngine->getCommandBuffer(), mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, mPushConstantsSize, pushConstants);
	}

	void VulkanRenderPipeline::bindIndexBuffer(GPUBuffer* buffer) {
		const VulkanBuffer* vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffer);
		vkCmdBindIndexBuffer(mVulkanEngine->getCommandBuffer(), vulkanBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	VulkanRenderPipeline::~VulkanRenderPipeline() {
		vkDestroyPipelineLayout(mVulkanEngine->getDevice(), mPipelineLayout, nullptr);
		vkDestroyPipeline(mVulkanEngine->getDevice(), mPipeline, nullptr);

		for (const VkDescriptorSetLayout& descriptorSetLayout: mDescriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(mVulkanEngine->getDevice(), descriptorSetLayout, nullptr);
		}
	}

	void VulkanRenderPipeline::draw_(const uint32_t vertexCount, const uint32_t firstVertex) {
		vkCmdDraw(mVulkanEngine->getCommandBuffer(), vertexCount, 1, firstVertex, 0);
	}

	void VulkanRenderPipeline::drawIndexed_(const uint32_t indexCount, const uint32_t firstIndex) {
		vkCmdDrawIndexed(mVulkanEngine->getCommandBuffer(), indexCount, 1, firstIndex, 0, 0);
	}

	VulkanRenderPipelineBuilder::VulkanRenderPipelineBuilder(VulkanEngine* vulkanEngine, const std::string& vertexShader, const std::string& fragmentShader) : RenderPipelineBuilder(vertexShader, fragmentShader), mVulkanEngine(vulkanEngine) {}

	std::unique_ptr<RenderPipeline> VulkanRenderPipelineBuilder::build() {
		return std::make_unique<VulkanRenderPipeline>(mVulkanEngine, this);
	}
}
