#version 460

#extension GL_EXT_buffer_reference : require

#include "voxel_game:chunk/buffers"

layout (push_constant) uniform PushConstants {
    mat4 viewProj;
} pushConstants;

layout (location = 0) out vec4 outColour;

void main() {
    Chunk chunk = chunks[gl_InstanceIndex];
    ChunkVertex vertex = chunk.vertexBuffer.vertices[gl_VertexIndex];
    gl_Position = pushConstants.viewProj * chunk.modelMatrix * vec4(vertex.x, vertex.y, vertex.z, 1);
    outColour = unpackUnorm4x8(vertex.colour);
}