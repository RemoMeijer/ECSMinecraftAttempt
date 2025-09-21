#ifndef WORLD_H
#define WORLD_H

#include <map>
#include <glm/glm.hpp>
#include "chunk.h"
#include "shader.h"

struct ivec2_compare {
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        if (a.y < b.y) return true;
        return false;
    }
};

// Using glm::ivec2 for chunk coordinates
using ChunkCoord = glm::ivec2;

class World {
public:
    World();
    void createChunk(int x, int z);

    void setBlock(int worldX, int worldY, int worldZ, BlockID type);
    
    void update();
    void render(Shader& shader);

private:
    std::map<ChunkCoord, Chunk, ivec2_compare> m_Chunks;
};

#endif