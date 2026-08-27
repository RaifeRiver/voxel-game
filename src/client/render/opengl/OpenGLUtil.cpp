#include "OpenGLUtil.h"

#include <vector>

#include "spirv_glsl.hpp"

#include "common/util/FileHelper.h"

namespace voxel_game::client::render::opengl::opengl_util {
	std::string loadShaderCode(const std::string& path) {
		std::vector<uint32_t> spirv = util::readFile<uint32_t>(path);
		spirv_cross::CompilerGLSL compiler(std::move(spirv));
		const spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();
		spirv_cross::CompilerGLSL::Options options;
		options.version = 430;
		compiler.set_common_options(options);

		auto flattenBindings = [&](const spirv_cross::SmallVector<spirv_cross::Resource>& resources) {
			for (const spirv_cross::Resource& resource : resources) {
				const uint32_t set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				const uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				compiler.unset_decoration(resource.id, spv::DecorationDescriptorSet);
				compiler.set_decoration(resource.id, spv::DecorationBinding, (set << 4) | binding);
			}
		};
		flattenBindings(shaderResources.sampled_images);
		flattenBindings(shaderResources.separate_images);
		flattenBindings(shaderResources.storage_images);
		flattenBindings(shaderResources.separate_samplers);
		flattenBindings(shaderResources.uniform_buffers);
		flattenBindings(shaderResources.storage_buffers);

		return compiler.compile();
	}
}
