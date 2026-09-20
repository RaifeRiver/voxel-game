#extension GL_EXT_buffer_reference : require

struct ChunkFace {
    uint pos;
    uint colour;
};

layout (buffer_reference, std430) readonly buffer ChunkFaceBuffer {
    ChunkFace faces[];
};

struct Chunk {
    mat4 modelMatrix;
    ivec4 boundingSphere;
    ChunkFaceBuffer faceBuffer;
    uint vertexCount;
};