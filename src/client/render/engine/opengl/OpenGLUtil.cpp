#include "OpenGLUtil.h"

#include <vector>

#include "glad/glad.h"
#include "spirv_glsl.hpp"

#include "common/util/FileHelper.h"

namespace voxel_game::client::render::engine::opengl::opengl_util {
	std::string convertShader(const std::vector<uint32_t>& shaderData) {
		spirv_cross::CompilerGLSL compiler(shaderData);
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

	std::vector<PushConstant> getPushConstants(const size_t shaderCount, const std::vector<uint32_t>* shaderData, const unsigned int program) {
		std::vector<PushConstant> pushConstants;
		for (size_t i = 0; i < shaderCount; i++) {
			spirv_cross::Compiler compiler(shaderData[i]);
			const spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();
			for (const spirv_cross::Resource& resource: shaderResources.push_constant_buffers) {
				const spirv_cross::SPIRType& type = compiler.get_type(resource.type_id);
				const std::string namePrefix = compiler.get_name(resource.id) + ".";
				for (uint32_t member = 0; member < type.member_types.size(); member++) {
					const std::string name = namePrefix + compiler.get_member_name(resource.base_type_id, member);
					const uint32_t offset = compiler.type_struct_member_offset(type, member);
					const int location = glGetUniformLocation(program, name.c_str());
					const spirv_cross::SPIRType& memberType = compiler.get_type(type.member_types[member]);
					PushConstantType pushConstantType = {
						.rows = memberType.vecsize,
						.columns = memberType.columns
					};
					switch (memberType.basetype) {
						case spirv_cross::SPIRType::Float:
							pushConstantType.type = PushConstantDataType::FLOAT;
							break;
						case spirv_cross::SPIRType::Double:
							pushConstantType.type = PushConstantDataType::DOUBLE;
							break;
						case spirv_cross::SPIRType::Int:
							pushConstantType.type = PushConstantDataType::INT;
							break;
						case spirv_cross::SPIRType::UInt:
							pushConstantType.type = PushConstantDataType::UINT;
							break;
						default:
							throw std::runtime_error("Unsupported push constant type");
					}
					for (auto& [n, l, s, t]: pushConstants) {
						if (n == name || l == location) {
							goto continue_member_loop;
						}
					}
					pushConstants.push_back({
						.name = name,
						.location = location,
						.offset = offset,
						.type = pushConstantType
					});
					continue_member_loop:;
				}
			}
		}
		return pushConstants;
	}

	void setPushConstantData(const std::vector<PushConstant>& pushConstants, void* data) {
		for (const PushConstant& pushConstant: pushConstants) {
			const auto [rows, columns, type] = pushConstant.type;
			switch (type) {
				case PushConstantDataType::FLOAT: {
					const auto* floatData = reinterpret_cast<float*>(static_cast<uint8_t*>(data) + pushConstant.offset);
					switch (columns) {
						case 1:
							switch (rows) {
								case 1:
									glUniform1f(pushConstant.location, floatData[0]);
									break;
								case 2:
									glUniform2f(pushConstant.location, floatData[0], floatData[1]);
									break;
								case 3:
									glUniform3f(pushConstant.location, floatData[0], floatData[1], floatData[2]);
									break;
								case 4:
									glUniform4f(pushConstant.location, floatData[0], floatData[1], floatData[2], floatData[3]);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 2:
							switch (rows) {
								case 2:
									glUniformMatrix2fv(pushConstant.location, 1, false, floatData);
									break;
								case 3:
									glUniformMatrix2x3fv(pushConstant.location, 1, false, floatData);
									break;
								case 4:
									glUniformMatrix2x4fv(pushConstant.location, 1, false, floatData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 3:
							switch (rows) {
								case 2:
									glUniformMatrix3x2fv(pushConstant.location, 1, false, floatData);
									break;
								case 3:
									glUniformMatrix3fv(pushConstant.location, 1, false, floatData);
									break;
								case 4:
									glUniformMatrix3x4fv(pushConstant.location, 1, false, floatData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 4:
							switch (rows) {
								case 2:
									glUniformMatrix4x2fv(pushConstant.location, 1, false, floatData);
									break;
								case 3:
									glUniformMatrix4x3fv(pushConstant.location, 1, false, floatData);
									break;
								case 4:
									glUniformMatrix4fv(pushConstant.location, 1, false, floatData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						default:
							throw std::runtime_error("Unsupported push constant type");
					}
					break;
				}
				case PushConstantDataType::DOUBLE: {
					const auto* doubleData = reinterpret_cast<double*>(static_cast<uint8_t*>(data) + pushConstant.offset);
					switch (columns) {
						case 1:
							switch (rows) {
								case 1:
									glUniform1d(pushConstant.location, doubleData[0]);
									break;
								case 2:
									glUniform2d(pushConstant.location, doubleData[0], doubleData[1]);
									break;
								case 3:
									glUniform3d(pushConstant.location, doubleData[0], doubleData[1], doubleData[2]);
									break;
								case 4:
									glUniform4d(pushConstant.location, doubleData[0], doubleData[1], doubleData[2], doubleData[3]);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 2:
							switch (rows) {
								case 2:
									glUniformMatrix2dv(pushConstant.location, 1, false, doubleData);
									break;
								case 3:
									glUniformMatrix2x3dv(pushConstant.location, 1, false, doubleData);
									break;
								case 4:
									glUniformMatrix2x4dv(pushConstant.location, 1, false, doubleData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 3:
							switch (rows) {
								case 2:
									glUniformMatrix3x2dv(pushConstant.location, 1, false, doubleData);
									break;
								case 3:
									glUniformMatrix3dv(pushConstant.location, 1, false, doubleData);
									break;
								case 4:
									glUniformMatrix3x4dv(pushConstant.location, 1, false, doubleData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						case 4:
							switch (rows) {
								case 2:
									glUniformMatrix4x2dv(pushConstant.location, 1, false, doubleData);
									break;
								case 3:
									glUniformMatrix4x3dv(pushConstant.location, 1, false, doubleData);
									break;
								case 4:
									glUniformMatrix4dv(pushConstant.location, 1, false, doubleData);
									break;
								default:
									throw std::runtime_error("Unsupported push constant type");
							}
							break;
						default:
							throw std::runtime_error("Unsupported push constant type");
					}
					break;
				}
				case PushConstantDataType::INT:
					if (columns == 1) {
						const auto* intData = reinterpret_cast<int32_t*>(static_cast<uint8_t*>(data) + pushConstant.offset);
						switch (rows) {
							case 1:
								glUniform1i(pushConstant.location, intData[0]);
								break;
							case 2:
								glUniform2i(pushConstant.location, intData[0], intData[1]);
								break;
							case 3:
								glUniform3i(pushConstant.location, intData[0], intData[1], intData[2]);
								break;
							case 4:
								glUniform4i(pushConstant.location, intData[0], intData[1], intData[2], intData[3]);
								break;
							default:
								throw std::runtime_error("Unsupported push constant type");
						}
					}
					else {
						throw std::runtime_error("Unsupported push constant type");
					}
					break;
				case PushConstantDataType::UINT:
					if (columns == 1) {
						const auto* uintData = reinterpret_cast<uint32_t*>(static_cast<uint8_t*>(data) + pushConstant.offset);
						switch (rows) {
							case 1:
								glUniform1ui(pushConstant.location, uintData[0]);
								break;
							case 2:
								glUniform2ui(pushConstant.location, uintData[0], uintData[1]);
								break;
							case 3:
								glUniform3ui(pushConstant.location, uintData[0], uintData[1], uintData[2]);
								break;
							case 4:
								glUniform4ui(pushConstant.location, uintData[0], uintData[1], uintData[2], uintData[3]);
								break;
							default:
								throw std::runtime_error("Unsupported push constant type");
						}
					}
					else {
						throw std::runtime_error("Unsupported push constant type");
					}
					break;
				default:
					throw std::runtime_error("Unsupported push constant type");
			}
		}
	}
}
