#include "chunksystem.h"
#include <vector>
#include <glad/glad.h>


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

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                // Ignore air blocks
                if (chunk.blocks[x][y][z] == BlockID::Air) {
                    continue;
                }

                float r = 0.5f, g = 0.5f, b = 0.5f; // stone colours
                if (chunk.blocks[x][y][z] == BlockID::Grass) { r = 0.0f; g = 1.0f; b = 0.0f; }
                else if (chunk.blocks[x][y][z] == BlockID::Dirt) { r = 0.6f; g = 0.4f; b = 0.2f; }

                // Check face +X (East)
                if (x == CHUNK_WIDTH - 1 || chunk.blocks[x + 1][y][z] == BlockID::Air) {
                    meshVertices.insert(meshVertices.end(), {
                        (float)x + 1, (float)y,     (float)z,     r,g,b,
                        (float)x + 1, (float)y + 1, (float)z,     r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y,     (float)z + 1, r,g,b,
                        (float)x + 1, (float)y,     (float)z,     r,g,b,
                    });
                }
                // Check face -X (West)
                if (x == 0 || chunk.blocks[x - 1][y][z] == BlockID::Air) {
                     meshVertices.insert(meshVertices.end(), {
                        (float)x, (float)y,     (float)z,     r,g,b,
                        (float)x, (float)y,     (float)z + 1, r,g,b,
                        (float)x, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x, (float)y + 1, (float)z,     r,g,b,
                        (float)x, (float)y,     (float)z,     r,g,b,
                    });
                }
                // Check face +Y (Top)
                if (y == CHUNK_HEIGHT - 1 || chunk.blocks[x][y + 1][z] == BlockID::Air) {
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y + 1, (float)z,     r,g,b,
                        (float)x,     (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z,     r,g,b,
                        (float)x,     (float)y + 1, (float)z,     r,g,b,
                    });
                }
                // Check face -Y (Bottom)
                if (y == 0 || chunk.blocks[x][y - 1][z] == BlockID::Air) {
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y, (float)z,     r,g,b,
                        (float)x + 1, (float)y, (float)z,     r,g,b,
                        (float)x + 1, (float)y, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y, (float)z + 1, r,g,b,
                        (float)x,     (float)y, (float)z + 1, r,g,b,
                        (float)x,     (float)y, (float)z,     r,g,b,
                    });
                }
                // Check face +Z (South)
                if (z == CHUNK_DEPTH - 1 || chunk.blocks[x][y][z + 1] == BlockID::Air) {
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y,     (float)z + 1, r,g,b,
                        (float)x + 1, (float)y,     (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z + 1, r,g,b,
                        (float)x,     (float)y + 1, (float)z + 1, r,g,b,
                        (float)x,     (float)y,     (float)z + 1, r,g,b,
                    });
                }
                // Check face -Z (North)
                if (z == 0 || chunk.blocks[x][y][z - 1] == BlockID::Air) {
                     meshVertices.insert(meshVertices.end(), {
                        (float)x,     (float)y,     (float)z, r,g,b,
                        (float)x,     (float)y + 1, (float)z, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z, r,g,b,
                        (float)x + 1, (float)y + 1, (float)z, r,g,b,
                        (float)x + 1, (float)y,     (float)z, r,g,b,
                        (float)x,     (float)y,     (float)z, r,g,b,
                    });
                }
            }
        }
    }

    chunk.vertexCount = meshVertices.size() / 6; // 6 floats per vertex (3 pos, 3 color)

    glGenVertexArrays(1, &chunk.VAO);
    glGenBuffers(1, &chunk.VBO);

    glBindVertexArray(chunk.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, chunk.VBO);

    // Upload data to the VBO
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(float), meshVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    chunk.isDirty = false;
}