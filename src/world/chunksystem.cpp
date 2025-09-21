#include "world/chunksystem.h"
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>


glm::vec2 getTextureCoordinates(BlockID blockType, int face) {
    const float ATLAS_STEP = 1.0f / 2.0f; // Since your atlas has 2 textures per row/col

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

void ChunkSystem::buildMesh(Chunk &chunk) {
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
                if (chunk.blocks[x][y][z] == BlockID::Air) continue;

               // Check face +Y (Top)
                if (y == CHUNK_HEIGHT - 1 || chunk.blocks[x][y + 1][z] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 0);
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
                if (y == 0 || chunk.blocks[x][y - 1][z] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 1);
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
                if (x == CHUNK_WIDTH - 1 || chunk.blocks[x + 1][y][z] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 2);
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
                if (x == 0 || chunk.blocks[x - 1][y][z] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 3);
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
                if (z == CHUNK_DEPTH - 1 || chunk.blocks[x][y][z + 1] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 4);
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
                if (z == 0 || chunk.blocks[x][y][z - 1] == BlockID::Air) {
                    glm::vec2 uv = getTextureCoordinates(currentBlock, 5);
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