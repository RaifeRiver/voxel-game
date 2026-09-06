#include "OpenGLRenderPipeline.h"

#include "OpenGLBuffer.h"
#include "glad/glad.h"

#include "OpenGLDescriptorSet.h"
#include "OpenGLUtil.h"
#include "client/render/engine/Shader.h"
#include "common/util/FileHelper.h"

namespace voxel_game::client::render::engine::opengl {
	unsigned int toOpenGLPrimitiveTopology(const PrimitiveTopology topology) {
		switch (topology) {
			case PrimitiveTopology::POINT_LIST:
				return GL_POINTS;
			case PrimitiveTopology::LINE_LIST:
				return GL_LINES;
			case PrimitiveTopology::LINE_STRIP:
				return GL_LINE_STRIP;
			case PrimitiveTopology::TRIANGLE_LIST:
				return GL_TRIANGLES;
			case PrimitiveTopology::TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			case PrimitiveTopology::TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			default:
				throw std::runtime_error("Unsupported primitive topology");
		}
	}

	unsigned int toOpenGLPolygonMode(const PolygonMode mode) {
		switch (mode) {
			case PolygonMode::FILL:
				return GL_FILL;
			case PolygonMode::LINE:
				return GL_LINE;
			case PolygonMode::POINT:
				return GL_POINT;
			default:
				throw std::runtime_error("Unsupported polygon mode");
		}
	}

	unsigned int toOpenGLCullFace(const CullMode mode) {
		switch (mode) {
			case CullMode::NONE:
				return 0;
			case CullMode::FRONT:
				return GL_FRONT;
			case CullMode::BACK:
				return GL_BACK;
			case CullMode::FRONT_AND_BACK:
				return GL_FRONT_AND_BACK;
			default:
				throw std::runtime_error("Unsupported cull mode");
		}
	}

	unsigned int toOpenGLFrontFace(const FrontFace face) {
		switch (face) {
			case FrontFace::COUNTER_CLOCKWISE:
				return GL_CCW;
			case FrontFace::CLOCKWISE:
				return GL_CW;
			default:
				throw std::runtime_error("Unsupported front face");
		}
	}

	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineBuilder* builder) {
		const std::vector<uint32_t>& vertexShaderData = builder->getVertexShader().getSPIRV();
		const std::vector<uint32_t>& fragmentShaderData = builder->getFragmentShader().getSPIRV();

		const std::string vertexShaderCode = opengl_util::convertShader(vertexShaderData);
		const std::string fragmentShaderCode = opengl_util::convertShader(fragmentShaderData);
		const char* vertexShaderCodeChars = vertexShaderCode.c_str();
		const char* fragmentShaderCodeChars = fragmentShaderCode.c_str();

		const unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCodeChars, nullptr);
		glCompileShader(vertexShader);

		const unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCodeChars, nullptr);
		glCompileShader(fragmentShader);

		mShaderProgram = glCreateProgram();
		glAttachShader(mShaderProgram, vertexShader);
		glAttachShader(mShaderProgram, fragmentShader);
		glLinkProgram(mShaderProgram);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		glGenVertexArrays(1, &mVertexArray);

		const std::vector<uint32_t> shaderData[] = {vertexShaderData, fragmentShaderData};
		mPushConstants = opengl_util::getPushConstants(2, shaderData, mShaderProgram);

		mPrimitiveTopology = toOpenGLPrimitiveTopology(builder->getPrimitiveTopology());
		mPolygonMode = toOpenGLPolygonMode(builder->getPolygonMode());
		mLineWidth = builder->getLineWidth();
		mCullFace = toOpenGLCullFace(builder->getCullMode());
		mFrontFace = toOpenGLFrontFace(builder->getFrontFace());
		mBlendMode = builder->getBlendMode();
	}

	void OpenGLRenderPipeline::bind() {
		glUseProgram(mShaderProgram);
		glBindVertexArray(mVertexArray);

		glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
		glLineWidth(mLineWidth);
		if (mCullFace == 0) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(mCullFace);
			glFrontFace(mFrontFace);
		}

		if (mBlendMode == BlendMode::NONE) {
			glDisable(GL_BLEND);
		}
		else {
			glEnable(GL_BLEND);
			if (mBlendMode == BlendMode::ADDITIVE) {
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
			}
			else if (mBlendMode == BlendMode::ALPHA) {
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
			}
			else {
				throw std::runtime_error("Unsupported blend mode");
			}
		}
	}

	void OpenGLRenderPipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		dynamic_cast<OpenGLDescriptorSet*>(descriptorSet)->bind(set);
	}

	void OpenGLRenderPipeline::setPushConstants(void* pushConstants) {
		opengl_util::setPushConstantData(mPushConstants, pushConstants);
	}

	void OpenGLRenderPipeline::bindIndexBuffer(GPUBuffer* buffer) {
		const OpenGLBuffer* openGLBuffer = dynamic_cast<OpenGLBuffer*>(buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLBuffer->getBuffer());
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		glDeleteVertexArrays(1, &mVertexArray);
		glDeleteProgram(mShaderProgram);
	}

	void OpenGLRenderPipeline::draw_(const uint32_t vertexCount, const uint32_t firstVertex) {
		glDrawArrays(mPrimitiveTopology, static_cast<int>(firstVertex), static_cast<int>(vertexCount));
	}

	void OpenGLRenderPipeline::drawIndexed_(const uint32_t indexCount, const uint32_t firstIndex) {
		glDrawElements(mPrimitiveTopology, static_cast<int>(indexCount), GL_UNSIGNED_INT, reinterpret_cast<void*>(firstIndex * 4));
	}

	OpenGLRenderPipelineBuilder::OpenGLRenderPipelineBuilder(const Shader& vertexShader, const Shader& fragmentShader) : RenderPipelineBuilder(vertexShader, fragmentShader) {}

	std::unique_ptr<RenderPipeline> OpenGLRenderPipelineBuilder::build() {
		return std::make_unique<OpenGLRenderPipeline>(this);
	}
}
