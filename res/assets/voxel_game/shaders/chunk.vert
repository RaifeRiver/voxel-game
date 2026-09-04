#version 460

#extension GL_EXT_buffer_reference : require

struct Vertex {
    float x;
    float y;
    float z;
    uint colour;
};

layout (buffer_reference, std430) readonly buffer VertexBuffer {
    Vertex vertices[];
};

layout (push_constant) uniform PushConstants {
    mat4 viewProj;
    VertexBuffer vertexBuffer;
} pushConstants;

layout (location = 0) out vec4 outColour;

void main() {
    Vertex vertex = pushConstants.vertexBuffer.vertices[gl_VertexIndex];
    gl_Position = pushConstants.viewProj * vec4(vertex.x, vertex.y, vertex.z, 1);
    outColour = unpackUnorm4x8(vertex.colour);
}