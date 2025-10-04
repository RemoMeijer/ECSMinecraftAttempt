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
    for (auto& [coord, chunk] : m_Chunks) {
        if (chunk.isDirty) {
            // Find neighbors for the current chunk
            ChunkCoord K_posX = ChunkCoord(coord.x + 1, coord.y);
            ChunkCoord K_negX = ChunkCoord(coord.x - 1, coord.y);
            ChunkCoord K_posZ = ChunkCoord(coord.x, coord.y + 1);
            ChunkCoord K_negZ = ChunkCoord(coord.x, coord.y - 1);

            // Get pointers to neighbors, or nullptr if they don't exist
            Chunk* p_posX = m_Chunks.count(K_posX) ? &m_Chunks.at(K_posX) : nullptr;
            Chunk* p_negX = m_Chunks.count(K_negX) ? &m_Chunks.at(K_negX) : nullptr;
            Chunk* p_posZ = m_Chunks.count(K_posZ) ? &m_Chunks.at(K_posZ) : nullptr;
            Chunk* p_negZ = m_Chunks.count(K_negZ) ? &m_Chunks.at(K_negZ) : nullptr;
            
            // Call buildMesh with the chunk and its neighbors
            ChunkSystem::buildMesh(chunk, p_posX, p_negX, p_posZ, p_negZ);
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