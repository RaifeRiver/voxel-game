#version 460

#extension GL_EXT_buffer_reference : require

#include "voxel_game:chunk/buffers"
#include "voxel_game:util/unpack_uint"

layout (push_constant) uniform PushConstants {
    mat4 viewProj;
} pushConstants;

layout (location = 0) out vec4 outColour;

const int indices[6] = {0, 1, 2, 2, 3, 0};

const vec3 vertexOffsets[24] = vec3[24](
    vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(0, 1, 1),
    vec3(1, 0, 0), vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0),
    vec3(0, 1, 1), vec3(1, 1, 1), vec3(1, 1, 0), vec3(0, 1, 0),
    vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1),
    vec3(1, 0, 1), vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1),
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), vec3(0, 1, 0)
);

void main() {
    Chunk chunk = chunks[gl_InstanceIndex];
    uint faceID = gl_VertexIndex / 6;
    uint vertexID = indices[gl_VertexIndex - faceID * 6];
    ChunkFace face = chunk.faceBuffer.faces[faceID];
    uvec4 posData = unpackUint4x8(face.pos);
    uint normal = posData.w;
    vec3 pos = vec3(posData.xyz) + vertexOffsets[normal * 4 + vertexID];
    gl_Position = pushConstants.viewProj * chunk.modelMatrix * vec4(pos, 1);
    outColour = unpackUnorm4x8(face.colour);
}