#version 460

layout (location = 0) out vec3 outColour;

uint indices[6] = uint[6](0, 1, 2, 2, 1, 3);

vec4 vertexPositions[4] = vec4[4](
    vec4(1.0f, -1.0f, 0.0f, 1.0f),
    vec4(1.0f, 1.0f, 0.0f, 1.0f),
    vec4(-1.0f, -1.0f, 0.0f, 1.0f),
    vec4(-1.0f, 1.0f, 0.0f, 1.0f)
);

void main() {
    uint index = indices[gl_VertexIndex];
    gl_Position = vertexPositions[index];
    outColour = vec3(0.3f, 0.7f, 1.0f);
}