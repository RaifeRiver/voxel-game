uvec4 unpackUint4x8(uint u) {
    return uvec4(u & 0xFF, (u >> 8) & 0xFF, (u >> 16) & 0xFF, (u >> 24) & 0xFF);
}