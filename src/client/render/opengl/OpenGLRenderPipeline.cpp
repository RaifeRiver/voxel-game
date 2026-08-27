#include "OpenGLRenderPipeline.h"

#include "glad/glad.h"

#include "OpenGLDescriptorSet.h"
#include "OpenGLUtil.h"

namespace voxel_game::client::render::opengl {
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

	OpenGLRenderPipeline::OpenGLRenderPipeline(const RenderPipelineBuilder* builder) {
		const std::string vertexShaderCode = opengl_util::loadShaderCode(builder->getVertexShader());
		const std::string fragmentShaderCode = opengl_util::loadShaderCode(builder->getFragmentShader());

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

		mPrimitiveTopology = toOpenGLPrimitiveTopology(builder->getPrimitiveTopology());
		mPolygonMode = toOpenGLPolygonMode(builder->getPolygonMode());
		mLineWidth = builder->getLineWidth();
	}

	void OpenGLRenderPipeline::bind() {
		glUseProgram(mShaderProgram);
		glBindVertexArray(mVertexArray);

		glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
		glLineWidth(mLineWidth);
	}

	void OpenGLRenderPipeline::bindDescriptorSet(const uint32_t set, DescriptorSet* descriptorSet) {
		dynamic_cast<OpenGLDescriptorSet*>(descriptorSet)->bind(set);
	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		glDeleteVertexArrays(1, &mVertexArray);
		glDeleteProgram(mShaderProgram);
	}

	void OpenGLRenderPipeline::draw_(const uint32_t vertexCount, const uint32_t firstVertex) {
		glDrawArrays(mPrimitiveTopology, static_cast<int>(firstVertex), static_cast<int>(vertexCount));
	}

	OpenGLRenderPipelineBuilder::OpenGLRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) : RenderPipelineBuilder(vertexShader, fragmentShader) {}

	std::unique_ptr<RenderPipeline> OpenGLRenderPipelineBuilder::build() {
		return std::make_unique<OpenGLRenderPipeline>(this);
	}
}
