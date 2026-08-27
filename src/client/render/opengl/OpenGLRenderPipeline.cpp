#include "OpenGLRenderPipeline.h"

#include "glad/glad.h"

#include "OpenGLUtil.h"

namespace voxel_game::client::render::opengl {
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
	}

	void OpenGLRenderPipeline::bind() {
		glUseProgram(mShaderProgram);
		glBindVertexArray(mVertexArray);
	}

	void OpenGLRenderPipeline::bindDescriptorSet(uint32_t set, DescriptorSet* descriptorSet) {

	}

	OpenGLRenderPipeline::~OpenGLRenderPipeline() {
		glDeleteVertexArrays(1, &mVertexArray);
		glDeleteProgram(mShaderProgram);
	}

	void OpenGLRenderPipeline::draw_(const uint32_t vertexCount, const uint32_t firstVertex) {
		glDrawArrays(GL_TRIANGLES, static_cast<int>(firstVertex), static_cast<int>(vertexCount));
	}

	OpenGLRenderPipelineBuilder::OpenGLRenderPipelineBuilder(const std::string& vertexShader, const std::string& fragmentShader) : RenderPipelineBuilder(vertexShader, fragmentShader) {}

	std::unique_ptr<RenderPipeline> OpenGLRenderPipelineBuilder::build() {
		return std::make_unique<OpenGLRenderPipeline>(this);
	}
}
