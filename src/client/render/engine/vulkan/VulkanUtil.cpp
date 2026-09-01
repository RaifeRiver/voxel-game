#include "VulkanUtil.h"

#include "spirv_cross.hpp"

namespace voxel_game::client::render::vulkan::vulkan_util {
	std::string vkResultString(const VkResult result) {
		switch (result) {
			case VK_SUCCESS:
				return "VK_SUCCESS";
			case VK_NOT_READY:
				return "VK_NOT_READY";
			case VK_TIMEOUT:
				return "VK_TIMEOUT";
			case VK_EVENT_SET:
				return "VK_EVENT_SET";
			case VK_EVENT_RESET:
				return "VK_EVENT_RESET";
			case VK_INCOMPLETE:
				return "VK_INCOMPLETE";
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				return "VK_ERROR_OUT_OF_HOST_MEMORY";
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
			case VK_ERROR_INITIALIZATION_FAILED:
				return "VK_ERROR_INITIALIZATION_FAILED";
			case VK_ERROR_DEVICE_LOST:
				return "VK_ERROR_DEVICE_LOST";
			case VK_ERROR_MEMORY_MAP_FAILED:
				return "VK_ERROR_MEMORY_MAP_FAILED";
			case VK_ERROR_LAYER_NOT_PRESENT:
				return "VK_ERROR_LAYER_NOT_PRESENT";
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				return "VK_ERROR_EXTENSION_NOT_PRESENT";
			case VK_ERROR_FEATURE_NOT_PRESENT:
				return "VK_ERROR_FEATURE_NOT_PRESENT";
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				return "VK_ERROR_INCOMPATIBLE_DRIVER";
			case VK_ERROR_TOO_MANY_OBJECTS:
				return "VK_ERROR_TOO_MANY_OBJECTS";
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				return "VK_ERROR_FORMAT_NOT_SUPPORTED";
			case VK_ERROR_FRAGMENTED_POOL:
				return "VK_ERROR_FRAGMENTED_POOL";
			case VK_ERROR_UNKNOWN:
				return "VK_ERROR_UNKNOWN";
			case VK_ERROR_VALIDATION_FAILED:
				return "VK_ERROR_VALIDATION_FAILED";
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				return "VK_ERROR_OUT_OF_POOL_MEMORY";
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
			case VK_ERROR_FRAGMENTATION:
				return "VK_ERROR_FRAGMENTATION";
			case VK_PIPELINE_COMPILE_REQUIRED:
				return "VK_PIPELINE_COMPILE_REQUIRED";
			case VK_ERROR_NOT_PERMITTED:
				return "VK_ERROR_NOT_PERMITTED";
			case VK_ERROR_SURFACE_LOST_KHR:
				return "VK_ERROR_SURFACE_LOST_KHR";
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
			case VK_SUBOPTIMAL_KHR:
				return "VK_SUBOPTIMAL_KHR";
			case VK_ERROR_OUT_OF_DATE_KHR:
				return "VK_ERROR_OUT_OF_DATE_KHR";
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
			case VK_ERROR_INVALID_SHADER_NV:
				return "VK_ERROR_INVALID_SHADER_NV";
			case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
				return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
			case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
				return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
			case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
				return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
			case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
				return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
			case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
				return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
			case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
				return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
			case VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT:
				return "VK_ERROR_PRESENT_TIMING_QUEUE_FULL_EXT";
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
			case VK_THREAD_IDLE_KHR:
				return "VK_THREAD_IDLE_KHR";
			case VK_THREAD_DONE_KHR:
				return "VK_THREAD_DONE_KHR";
			case VK_OPERATION_DEFERRED_KHR:
				return "VK_OPERATION_DEFERRED_KHR";
			case VK_OPERATION_NOT_DEFERRED_KHR:
				return "VK_OPERATION_NOT_DEFERRED_KHR";
			case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
				return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
				return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
			case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
				return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
			case VK_PIPELINE_BINARY_MISSING_KHR:
				return "VK_PIPELINE_BINARY_MISSING_KHR";
			case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
				return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
			default:
				return "Unknown VkResult";
		}
	}

	std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts(const VulkanEngine* vulkanEngine, const size_t shaderCount, const std::vector<uint32_t>* shaderData) {
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
		auto processResources = [&](const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& resources, const VkShaderStageFlags shaderStage, const VkDescriptorType descriptorType) {
			for (const spirv_cross::Resource& sampledImage: resources) {
				const uint32_t set = compiler.get_decoration(sampledImage.id, spv::DecorationDescriptorSet);
				const uint32_t binding = compiler.get_decoration(sampledImage.id, spv::DecorationBinding);
				if (descriptorSetLayoutBindings.size() <= set) {
					descriptorSetLayoutBindings.resize(set + 1);
				}
				bool found = false;
				for (VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding: descriptorSetLayoutBindings[set]) {
					if (descriptorSetLayoutBinding.binding == binding) {
						if (descriptorSetLayoutBinding.descriptorType != descriptorType) {
							throw std::runtime_error("Shader binding conflict, set = " + std::to_string(set) + ", binding = " + std::to_string(binding));
						}
						found = true;
						descriptorSetLayoutBinding.stageFlags |= shaderStage;
						break;
					}
				}
				if (!found) {
					descriptorSetLayoutBindings[set].push_back({
						.binding = binding,
						.descriptorType = descriptorType,
						.descriptorCount = 1,
						.stageFlags = shaderStage
					});
				}
			}
		};
		for (size_t i = 0; i < shaderCount; i++) {
			const spirv_cross::Compiler compiler(shaderData[i]);
			const spirv_cross::SmallVector<spirv_cross::EntryPoint> entryPoints = compiler.get_entry_points_and_stages();
			VkShaderStageFlags shaderStage = 0;
			for (const auto& [name, executionModel]: entryPoints) {
				switch (executionModel) {
					case spv::ExecutionModelVertex:
						shaderStage |= VK_SHADER_STAGE_VERTEX_BIT;
						break;
					case spv::ExecutionModelFragment:
						shaderStage |= VK_SHADER_STAGE_FRAGMENT_BIT;
						break;
					case spv::ExecutionModelGLCompute:
						shaderStage |= VK_SHADER_STAGE_COMPUTE_BIT;
						break;
					default:
						throw std::runtime_error("Unsupported execution model: " + std::to_string(executionModel));
				}
			}
			const spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();
			processResources(compiler, shaderResources.sampled_images, shaderStage, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			processResources(compiler, shaderResources.separate_images, shaderStage, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE);
			processResources(compiler, shaderResources.storage_images, shaderStage, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
			processResources(compiler, shaderResources.separate_samplers, shaderStage, VK_DESCRIPTOR_TYPE_SAMPLER);
			processResources(compiler, shaderResources.uniform_buffers, shaderStage, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			processResources(compiler, shaderResources.storage_buffers, shaderStage, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		}

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		descriptorSetLayouts.resize(descriptorSetLayoutBindings.size());
		for (size_t i = 0; i < descriptorSetLayoutBindings.size(); i++) {
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings[i].size()),
				.pBindings = descriptorSetLayoutBindings[i].data()
			};
			vkCheck(vkCreateDescriptorSetLayout(vulkanEngine->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayouts[i]));
		}
		return descriptorSetLayouts;
	}

	std::vector<VkPushConstantRange> getPushConstantRanges(const size_t shaderCount, const std::vector<uint32_t>* shaderData) {
		std::vector<VkPushConstantRange> pushConstantRanges;
		VkShaderStageFlags shaderStages = 0;
		size_t maxSize = 0;
		for (size_t i = 0; i < shaderCount; i++) {
			const spirv_cross::Compiler compiler(shaderData[i]);
			const spirv_cross::SmallVector<spirv_cross::EntryPoint> entryPoints = compiler.get_entry_points_and_stages();
			for (const auto& [name, executionModel]: entryPoints) {
				switch (executionModel) {
					case spv::ExecutionModelVertex:
						shaderStages |= VK_SHADER_STAGE_VERTEX_BIT;
						break;
					case spv::ExecutionModelFragment:
						shaderStages |= VK_SHADER_STAGE_FRAGMENT_BIT;
						break;
					case spv::ExecutionModelGLCompute:
						shaderStages |= VK_SHADER_STAGE_COMPUTE_BIT;
						break;
					default:
						throw std::runtime_error("Unsupported execution model: " + std::to_string(executionModel));
				}
			}
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			for (const spirv_cross::Resource& pushConstant: resources.push_constant_buffers) {
				const spirv_cross::SPIRType& type = compiler.get_type(pushConstant.type_id);
				size_t size = compiler.get_declared_struct_size(type);
				maxSize = std::max(maxSize, size);
			}
		}
		if (maxSize != 0) {
			pushConstantRanges.push_back(VkPushConstantRange{
				.stageFlags = shaderStages,
				.size = static_cast<uint32_t>(maxSize)
			});
		}
		return pushConstantRanges;
	}
}
