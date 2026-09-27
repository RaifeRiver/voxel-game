#include "voxel_game:util/indirect_command"

struct ChunkFace {
    uint pos;
    uint colour;
};

#ifdef VG_ENGINE_BUFFER_REFERENCE
#extension GL_EXT_buffer_reference : require

layout (buffer_reference, std430) readonly buffer ChunkFaceBuffer {
    ChunkFace faces[];
};
#else
layout (set = 0, binding = 4, std430) readonly buffer ChunkFaceBuffer {
    ChunkFace faces[];
} faceBuffer;
#endif

struct Chunk {
#ifdef VG_ENGINE_BUFFER_REFERENCE
    ChunkFaceBuffer faceBuffer;
#else
    uint faceOffset;
    uint padding;
#endif
    uint vertexCount;
};

#ifdef VG_ENGINE_BUFFER_REFERENCE
ChunkFace getFace(Chunk chunk, uint id) {
    return chunk.faceBuffer.faces[id];
}
#else
ChunkFace getFace(Chunk chunk, uint id) {
    return faceBuffer.faces[chunk.faceOffset + id];
}
#endif

struct FrameChunk {
    mat4 modelMatrix;
    ivec4 boundingSphere;
};

layout (set = 0, binding = 0, std430) readonly buffer ChunkBuffer {
    Chunk chunks[];
};

layout (set = 0, binding = 1, std430) writeonly buffer IndirectCommandBuffer {
    IndirectCommand indirectCommands[];
};

layout (set = 0, binding = 2, std430) buffer CountBuffer {
    uint commandCount;
};

layout (set = 0, binding = 3, std430) readonly buffer FrameChunkBuffer {
    FrameChunk frameChunks[];
};