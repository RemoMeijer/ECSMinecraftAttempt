#ifndef CHUNKSYSTEM_H
#define CHUNKSYSTEM_H

#include "chunk.h"

namespace ChunkSystem {
    void generate(Chunk& chunk);
    void buildMesh(Chunk& chunk, Chunk* neighbourPosX, Chunk* neighbourNegX, Chunk* neighbourPosY, Chunk* neighbourNegY);
    void unloadMesh(Chunk& chunk);
}

#endif