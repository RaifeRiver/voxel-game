#extension GL_EXT_buffer_reference : require

struct ChunkVertex {
    float x;
    float y;
    float z;
    uint colour;
};

layout (buffer_reference, std430) readonly buffer ChunkVertexBuffer {
    ChunkVertex vertices[];
};

struct Chunk {
    mat4 modelMatrix;
    ivec4 boundingSphere;
    ChunkVertexBuffer vertexBuffer;
    uint vertexCount;
};