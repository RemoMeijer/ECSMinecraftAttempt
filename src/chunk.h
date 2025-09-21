#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"

constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 256;
constexpr int CHUNK_DEPTH = 16;

class Chunk {
public:
    Chunk();
    ~Chunk();

    void generate();
    void buildMesh();
    void draw();

private:
    BlockID m_Blocks[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_DEPTH];
    unsigned int m_VAO, m_VBO;
    int m_VertexCount;
};

#endif