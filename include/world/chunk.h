#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_DEPTH = 16;

struct Chunk {
    BlockID blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];

    unsigned int VAO = 0;
    unsigned int VBO = 0;
    int vertexCount = 0;

    bool isDirty = true;
};

#endif