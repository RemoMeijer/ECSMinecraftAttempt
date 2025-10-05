// src/world/raycast.cpp
#include "world/raycast.h"
#include <cmath>

namespace RaycastSystem {

std::optional<RaycastResult> cast(const World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
    if (glm::length(direction) == 0.0f) {
        return std::nullopt;
    }

    glm::vec3 step = glm::normalize(direction) * 0.2f;
    glm::vec3 currentPos = origin;
    glm::ivec3 lastBlockPos = {floor(origin.x), floor(origin.y), floor(origin.z)};

    for (float traveled = 0.0f; traveled < maxDistance; traveled += 0.2f) {
        currentPos += step;
        glm::ivec3 currentBlockPos = {floor(currentPos.x), floor(currentPos.y), floor(currentPos.z)};

        // Check if we've entered a new block
        if (currentBlockPos != lastBlockPos) {
            if (world.getBlock(currentBlockPos.x, currentBlockPos.y, currentBlockPos.z) != BlockID::Air) {
                // We hit a solid block
                return RaycastResult{currentBlockPos, lastBlockPos};
            }
            lastBlockPos = currentBlockPos;
        }
    }

    return std::nullopt; // No block was hit
}

} // namespace RaycastSystem