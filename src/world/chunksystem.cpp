#include "world/chunksystem.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>


glm::vec2 getTextureCoordinates(BlockID blockType, int face) {
    const float ATLAS_STEP = 1.0f / 2.0f;

    switch (blockType) {
        case BlockID::Grass:
            if (face == 0) return {1.0f * ATLAS_STEP, 0.0f * ATLAS_STEP}; // Top face -> Grass Top (0, 1)
            if (face == 1) return {0.0f * ATLAS_STEP, 0.0f * ATLAS_STEP}; // Bottom face -> Dirt (0, 0)
            return {0.0f * ATLAS_STEP, 1.0f * ATLAS_STEP};              // Sides -> Grass Side (1, 0)
        
        case BlockID::Dirt:
            return {0.0f * ATLAS_STEP, 0.0f * ATLAS_STEP}; // All faces -> Dirt (0, 0)

        case BlockID::Stone:
            return {1.0f * ATLAS_STEP, 1.0f * ATLAS_STEP}; // All faces -> Stone (1, 1)
        
        default:
            return {0.0f, 0.0f};
    }
}

void ChunkSystem::generate(Chunk &chunk) {
    int groundHeight = 64; // Y-level for flat world

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                if (y < groundHeight - 2) {
                    chunk.blocks[x][y][z] = BlockID::Stone;
                } else if (y == groundHeight -2 || y == groundHeight -1) {
                    chunk.blocks[x][y][z] = BlockID::Dirt;
                } else if (y == groundHeight) {
                    chunk.blocks[x][y][z] = BlockID::Grass;
                } else if (y > groundHeight) {
                    chunk.blocks[x][y][z] = BlockID::Air;
                }
            }
        }
    }
}

void ChunkSystem::buildMesh(Chunk &chunk, Chunk* neighbor_posX, Chunk* neighbor_negX, Chunk* neighbor_posZ, Chunk* neighbor_negZ) {
    if (chunk.VBO != 0) {
        glDeleteBuffers(1, &chunk.VBO);
    }
    if (chunk.VAO != 0) {
        glDeleteVertexArrays(1, &chunk.VAO);
    }

    std::vector<float> meshVertices;
    const float ATLAS_STEP = 1.0f / 2.0f;

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                BlockID currentBlock = chunk.blocks[x][y][z];
                if (currentBlock == BlockID::Air) continue;

                // Helper lambda to check if a block is solid (not Air)
                auto isBlockSolid = [&](int bx, int by, int bz) {
                    // Check within the current chunk
                    if (bx >= 0 && bx < CHUNK_WIDTH && by >= 0 && by < CHUNK_HEIGHT && bz >= 0 && bz < CHUNK_DEPTH) {
                        return chunk.blocks[bx][by][bz] != BlockID::Air;
                    }
                    // Check neighbor chunks
                    if (by < 0 || by >= CHUNK_HEIGHT) return false; // Out of vertical bounds

                    if (bx < 0) return neighbor_negX && neighbor_negX->blocks[CHUNK_WIDTH + bx][by][bz] != BlockID::Air;
                    if (bx >= CHUNK_WIDTH) return neighbor_posX && neighbor_posX->blocks[bx - CHUNK_WIDTH][by][bz] != BlockID::Air;
                    if (bz < 0) return neighbor_negZ && neighbor_negZ->blocks[bx][by][CHUNK_DEPTH + bz] != BlockID::Air;
                    if (bz >= CHUNK_DEPTH) return neighbor_posZ && neighbor_posZ->blocks[bx][by][bz - CHUNK_DEPTH] != BlockID::Air;
                    
                    return false; // Should not be reached
                };

                // Check face +Y (Top)
                if (!isBlockSolid(x, y + 1, z)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 0);
                    // Add vertices for top face...
                    meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y + 1, (float)z,     uv.x,            uv.y + ATLAS_STEP,
                        (float)x,     (float)y + 1, (float)z + 1, uv.x,            uv.y,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y + 1, (float)z,     uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x,     (float)y + 1, (float)z,     uv.x,            uv.y + ATLAS_STEP,
                    });
                }
                // Check face -Y (Bottom)
                if (!isBlockSolid(x, y - 1, z)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 1);
                    // Add vertices for bottom face...
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y, (float)z,     uv.x,            uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y, (float)z,     uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x,     (float)y, (float)z + 1, uv.x,            uv.y,
                        (float)x,     (float)y, (float)z,     uv.x,            uv.y + ATLAS_STEP,
                    });
                }
                // Check face +X (East)
                if (!isBlockSolid(x + 1, y, z)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 2);
                    // Add vertices for +X face...
                    meshVertices.insert(meshVertices.end(), {
                        (float)x + 1, (float)y,     (float)z,     uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y + 1, (float)z,     uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x,            uv.y,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x,            uv.y,
                        (float)x + 1, (float)y,     (float)z + 1, uv.x,            uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y,     (float)z,     uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                    });
                }
                // Check face -X (West)
                if (!isBlockSolid(x - 1, y, z)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 3);
                    // Add vertices for -X face...
                     meshVertices.insert(meshVertices.end(), {
                        (float)x, (float)y,     (float)z,     uv.x,            uv.y + ATLAS_STEP,
                        (float)x, (float)y + 1, (float)z,     uv.x,            uv.y,
                        (float)x, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x, (float)y,     (float)z + 1, uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x, (float)y,     (float)z,     uv.x,            uv.y + ATLAS_STEP,
                    });
                }
                // Check face +Z (South)
                if (!isBlockSolid(x, y, z + 1)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 4);
                    // Add vertices for +Z face...
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y,     (float)z + 1, uv.x,            uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y,     (float)z + 1, uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y + 1, (float)z + 1, uv.x + ATLAS_STEP, uv.y,
                        (float)x,     (float)y + 1, (float)z + 1, uv.x,            uv.y,
                        (float)x,     (float)y,     (float)z + 1, uv.x,            uv.y + ATLAS_STEP,
                    });
                }
                // Check face -Z (North)
                if (!isBlockSolid(x, y, z - 1)) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 5);
                    // Add vertices for -Z face...
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y,     (float)z, uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                        (float)x,     (float)y + 1, (float)z, uv.x + ATLAS_STEP, uv.y,
                        (float)x + 1, (float)y + 1, (float)z, uv.x,            uv.y,
                        (float)x + 1, (float)y + 1, (float)z, uv.x,            uv.y,
                        (float)x + 1, (float)y,     (float)z, uv.x,            uv.y + ATLAS_STEP,
                        (float)x,     (float)y,     (float)z, uv.x + ATLAS_STEP, uv.y + ATLAS_STEP,
                    });
                }
            }
        }
    }

    chunk.vertexCount = meshVertices.size() / 5;

    glGenVertexArrays(1, &chunk.VAO);
    glGenBuffers(1, &chunk.VBO);

    glBindVertexArray(chunk.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);

    // Upload data to the VBO
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(float), meshVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    chunk.isDirty = false;
}

void ChunkSystem::unloadMesh(Chunk &chunk) {
    if (chunk.VBO != 0) {
        glDeleteBuffers(1, &chunk.VBO);
        chunk.VBO = 0;
    }
    if (chunk.VAO != 0) {
        glDeleteVertexArrays(1, &chunk.VAO);
        chunk.VAO = 0;
    }
}