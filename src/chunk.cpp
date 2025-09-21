#include "chunk.h"
#include <vector>
#include <glad/glad.h>

Chunk::Chunk() {
    generate();
}

void Chunk::generate() {
    int groundHeight = 64; // Y-level for flat world

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                if (y < groundHeight - 2) {
                    m_Blocks[x][y][z] = BlockID::Stone;
                } else if (y == groundHeight -2 || y == groundHeight -1) {
                    m_Blocks[x][y][z] = BlockID::Dirt;
                } else if (y == groundHeight) {
                    m_Blocks[x][y][z] = BlockID::Grass;
                } else if (y > groundHeight) {
                    m_Blocks[x][y][z] = BlockID::Air;
                }
            }
        }
    }
}

void Chunk::buildMesh() {
    std::vector<float> meshVertices;

    for (int x = 0; x < CHUNK_WIDTH; ++x) {
        for (int z = 0; z < CHUNK_DEPTH; ++z) {
            for (int y = 0; y < CHUNK_HEIGHT; ++y) {
                // Ignore air blocks
                if (m_Blocks[x][y][z] == BlockID::Air) {
                    continue;
                }

                float r = 0.5f, g = 0.5f, b = 0.5f; // stone colours
                if (m_Blocks[x][y][z] == BlockID::Grass) { r == 0.0f; g = 1.0f; b = 0.0f; }
                else if (m_Blocks[x][y][z] == BlockID::Dirt) { r = 0.6f; g = 0.4f; b = 0.2f; }

                // Check face +X (East)
                if (x == CHUNK_WIDTH - 1 || m_Blocks[x + 1][y][z] == BlockID::Air) {
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
                if (x == 0 || m_Blocks[x - 1][y][z] == BlockID::Air) {
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
                if (y == CHUNK_HEIGHT - 1 || m_Blocks[x][y + 1][z] == BlockID::Air) {
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
                if (y == 0 || m_Blocks[x][y - 1][z] == BlockID::Air) {
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
                if (z == CHUNK_DEPTH - 1 || m_Blocks[x][y][z + 1] == BlockID::Air) {
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
                if (z == 0 || m_Blocks[x][y][z - 1] == BlockID::Air) {
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

    m_VertexCount = meshVertices.size() / 6; // 6 floats per vertex (3 pos, 3 color)

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // Upload data to the VBO
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(float), meshVertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Chunk::draw() {
    if (m_VertexCount == 0) return; // Don't draw if there's no mesh

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    glBindVertexArray(0);
}

Chunk::~Chunk() {
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
    }
}