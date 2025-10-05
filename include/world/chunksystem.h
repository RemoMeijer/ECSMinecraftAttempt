#ifndef CHUNKSYSTEM_H
#define CHUNKSYSTEM_H

#include "chunk.h"
#include "world/FastNoiseLite.h"

namespace ChunkSystem {
    void generate(Chunk& chunk, int chunkX, int chunkZ, FastNoiseLite& noise, FastNoiseLite& detailNoise);
    void buildMesh(Chunk& chunk, Chunk* neighbourPosX, Chunk* neighbourNegX, Chunk* neighbourPosY, Chunk* neighbourNegY);
    void unloadMesh(Chunk& chunk);
}

#endif