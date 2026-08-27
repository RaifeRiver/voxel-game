#include "VulkanRenderPipeline.h"

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
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &vertexShaderModuleCreateInfo, nullptr, &vertexShaderModule));

		std::vector<uint32_t> fragmentShaderData = util::readFile<uint32_t>(builder->getFragmentShader());
		const VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = fragmentShaderData.size() * sizeof(uint32_t),
			.pCode = fragmentShaderData.data()
		};
		VkShaderModule fragmentShaderModule;
		vulkan_util::vkCheck(vkCreateShaderModule(vulkanEngine->getDevice(), &fragmentShaderModuleCreateInfo, nullptr, &fragmentShaderModule));

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

		const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(mDescriptorSetLayouts.size()),
			.pSetLayouts = mDescriptorSetLayouts.data()
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

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo pipelineColourBlendStateCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = false,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &pipelineColorBlendAttachmentState
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

		vulkan_util::vkCheck(vkCreateGraphicsPipelines(vulkanEngine->getDevice(), nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline));
	}

	VulkanRenderPipelineBuilder::VulkanRenderPipelineBuilder(VulkanEngine* vulkanEngine, const std::string& vertexShader, const std::string& fragmentShader) : RenderPipelineBuilder(vertexShader, fragmentShader), mVulkanEngine(vulkanEngine) {}

	std::unique_ptr<RenderPipeline> VulkanRenderPipelineBuilder::build() {
		return std::make_unique<VulkanRenderPipeline>(mVulkanEngine, this);
	}
}
