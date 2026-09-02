#pragma once

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include "DescriptorSet.h"
#include "Shader.h"

namespace voxel_game::client::render::engine {
	enum class DescriptorType {
		SAMPLED_TEXTURE,
		TEXTURE,
		IMAGE,
		SAMPLER,
		UNIFORM_BUFFER,
		STORAGE_BUFFER
	};

	class DescriptorAllocator {
	public:
		virtual void clearDescriptors() = 0;

		virtual std::unique_ptr<DescriptorSet> allocate() = 0;

		virtual ~DescriptorAllocator() = default;
	};

	using DescriptorBinding = std::pair<uint32_t, DescriptorType>;

	class DescriptorAllocatorBuilder {
	public:
		DescriptorAllocatorBuilder* addBinding(uint32_t binding, DescriptorType type);

		virtual std::unique_ptr<DescriptorAllocator> build(uint32_t maxSets, ShaderStage shaderStages) = 0;

		virtual ~DescriptorAllocatorBuilder() = default;

	protected:
		std::vector<DescriptorBinding> mBindings;
	};
}
