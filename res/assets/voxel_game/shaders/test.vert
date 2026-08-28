#version 460

layout (location = 0) out vec4 outColour;

struct Vertex {
    vec4 pos;
    vec4 colour;
};

layout (set = 0, binding = 0) readonly buffer VertexBuffer {
    Vertex vertices[];
} vertices;

void main() {
    Vertex vertex = vertices.vertices[gl_VertexIndex];
    gl_Position = vertex.pos;
    outColour = vertex.colour;
}