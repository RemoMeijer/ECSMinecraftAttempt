#include "world/world.h"
#include "world/chunksystem.h"
#include <glm/gtc/matrix_transform.hpp>


World::World() {}

void World::createChunk(int x, int z) {
    ChunkCoord coord(x, z);
    m_Chunks[coord] = Chunk(); // Create a new chunk
    ChunkSystem::generate(m_Chunks.at(coord));
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockID type) {
    // Convert world coordinates to chunk and local block coordinates
    ChunkCoord chunkCoord(floor(worldX / CHUNK_WIDTH), floor(worldZ / CHUNK_DEPTH));
    int localX = worldX % CHUNK_WIDTH;
    int localZ = worldZ % CHUNK_DEPTH;

    // If the chunk exists, set the block and mark it dirty
    if (m_Chunks.count(chunkCoord)) {
        m_Chunks.at(chunkCoord).blocks[localX][worldY][localZ] = type;
        m_Chunks.at(chunkCoord).isDirty = true;
    }
}

void World::update() {
    // This is our "Meshing System". Find all dirty chunks and rebuild their mesh.
    for (auto& pair : m_Chunks) {
        if (pair.second.isDirty) {
            ChunkSystem::buildMesh(pair.second);
        }
    }
}

void World::render(Shader& shader) {
    // This is our "Render System". Draw all chunks that have a mesh.
    for (auto const& [coord, chunk] : m_Chunks) {
        if (chunk.vertexCount > 0) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(coord.x * CHUNK_WIDTH, 0, coord.y * CHUNK_DEPTH));
            shader.setMat4("model", model);
            
            glBindVertexArray(chunk.VAO);
            glDrawArrays(GL_TRIANGLES, 0, chunk.vertexCount);
        }
    }
}