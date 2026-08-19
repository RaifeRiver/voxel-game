#include "VulkanDescriptorAllocator.h"

#include <ranges>
#include <unordered_map>

#include "VulkanDescriptorSet.h"
#include "VulkanEngine.h"
#include "VulkanShader.h"
#include "VulkanUtil.h"

namespace voxel_game::client::render::vulkan {
	VkDescriptorType toVKDescriptorType(const DescriptorType type) {
		switch (type) {
			case DescriptorType::SAMPLED_TEXTURE:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::TEXTURE:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case DescriptorType::IMAGE:
				return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case DescriptorType::SAMPLER:
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			case DescriptorType::UNIFORM_BUFFER:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::STORAGE_BUFFER:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			default:
				throw std::runtime_error("Unsupported descriptor type");
		}
	}

	VkImageLayout toVKImageLayout(const DescriptorType type) {
		switch (type) {
			case DescriptorType::SAMPLED_TEXTURE:
			case DescriptorType::TEXTURE:
				return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case DescriptorType::IMAGE:
				return VK_IMAGE_LAYOUT_GENERAL;
			default:
				throw std::runtime_error("Unsupported descriptor type");
		}
	}

	VulkanDescriptorAllocator::VulkanDescriptorAllocator(VulkanEngine* vulkanEngine, const std::vector<DescriptorBinding>& bindings, const uint32_t maxSets, const ShaderStage shaderStages) : mVulkanEngine(vulkanEngine), mBindings(bindings) {
		std::unordered_map<DescriptorType, uint32_t> typeCounts;
		for (const auto &type: bindings | std::views::values) {
			auto it = typeCounts.find(type);
			if (it != typeCounts.end()) {
				it->second++;
			}
			else {
				typeCounts[type] = 1;
			}
		}
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		descriptorPoolSizes.reserve(typeCounts.size());
		for (auto& [type, count] : typeCounts) {
			descriptorPoolSizes.push_back({
				.type = toVKDescriptorType(type),
				.descriptorCount = count * maxSets
			});
		}

		const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = maxSets,
			.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size()),
			.pPoolSizes = descriptorPoolSizes.data()
		};
		vulkan_util::vkCheck(vkCreateDescriptorPool(mVulkanEngine->getDevice(), &descriptorPoolCreateInfo, nullptr, &mDescriptorPool));

		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
		descriptorSetLayoutBindings.reserve(bindings.size());
		for (const auto&[binding, type] : bindings) {
			descriptorSetLayoutBindings.push_back({
				.binding = binding,
				.descriptorType = toVKDescriptorType(type),
				.descriptorCount = 1,
				.stageFlags = toVKShaderStage(shaderStages)
			});
		}
		const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size()),
			.pBindings = descriptorSetLayoutBindings.data()
		};
		vulkan_util::vkCheck(vkCreateDescriptorSetLayout(mVulkanEngine->getDevice(), &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout));
	}

	void VulkanDescriptorAllocator::clearDescriptors() {
		vkResetDescriptorPool(mVulkanEngine->getDevice(), mDescriptorPool, 0);
	}

	std::unique_ptr<DescriptorSet> VulkanDescriptorAllocator::allocate() {
		const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = mDescriptorPool,
			.descriptorSetCount = 1,
			.pSetLayouts = &mDescriptorSetLayout
		};
		VkDescriptorSet descriptorSet;
		vulkan_util::vkCheck(vkAllocateDescriptorSets(mVulkanEngine->getDevice(), &descriptorSetAllocateInfo, &descriptorSet));
		return std::make_unique<VulkanDescriptorSet>(mVulkanEngine, this, mDescriptorPool, descriptorSet);
	}

	DescriptorType VulkanDescriptorAllocator::getDescriptorType(const uint32_t binding) {
		for (auto& [b, type] : mBindings) {
			if (b == binding) {
				return type;
			}
		}
		throw std::runtime_error("No binding at index " + std::to_string(binding));
	}

	VulkanDescriptorAllocator::~VulkanDescriptorAllocator() {
		vkDestroyDescriptorSetLayout(mVulkanEngine->getDevice(), mDescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(mVulkanEngine->getDevice(), mDescriptorPool, nullptr);
	}

	VulkanDescriptorAllocatorBuilder::VulkanDescriptorAllocatorBuilder(VulkanEngine* vulkanEngine) : mVulkanEngine(vulkanEngine) {}

	std::unique_ptr<DescriptorAllocator> VulkanDescriptorAllocatorBuilder::build(const uint32_t maxSets, const ShaderStage shaderStages) {
		return std::make_unique<VulkanDescriptorAllocator>(mVulkanEngine, mBindings, maxSets, shaderStages);
	}
}
