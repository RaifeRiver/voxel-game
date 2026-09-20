#include "voxel_game:chunk/chunk_format"
#include "voxel_game:util/indirect_command"

layout (set = 0, binding = 0, std430) readonly buffer ChunkBuffer {
    Chunk chunks[];
};

layout (set = 0, binding = 1, std430) writeonly buffer IndirectCommandBuffer {
    IndirectCommand indirectCommands[];
};

layout (set = 0, binding = 2, std430) buffer CountBuffer {
    uint commandCount;
};