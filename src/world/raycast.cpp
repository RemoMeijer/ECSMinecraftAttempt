// src/world/raycast.cpp
#include "world/raycast.h"
#include <cmath>
#include <limits> // Required for std::numeric_limits

namespace RaycastSystem {

std::optional<RaycastResult> cast(World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
    if (glm::length(direction) == 0.0f) {
        return std::nullopt; // Cannot cast a ray with no direction
    }

    glm::ivec3 currentBlockPos = {floor(origin.x), floor(origin.y), floor(origin.z)};
    glm::vec3 rayDir = glm::normalize(direction);

    glm::vec3 deltaDist = {
        (rayDir.x == 0) ? std::numeric_limits<float>::infinity() : abs(1.0f / rayDir.x),
        (rayDir.y == 0) ? std::numeric_limits<float>::infinity() : abs(1.0f / rayDir.y),
        (rayDir.z == 0) ? std::numeric_limits<float>::infinity() : abs(1.0f / rayDir.z)
    };

    glm::vec3 sideDist;
    glm::ivec3 step;

    if (rayDir.x < 0) {
        step.x = -1;
        sideDist.x = (origin.x - currentBlockPos.x) * deltaDist.x;
    } else {
        step.x = 1;
        sideDist.x = (currentBlockPos.x + 1.0f - origin.x) * deltaDist.x;
    }
    // Repeat for Y and Z
    if (rayDir.y < 0) {
        step.y = -1;
        sideDist.y = (origin.y - currentBlockPos.y) * deltaDist.y;
    } else {
        step.y = 1;
        sideDist.y = (currentBlockPos.y + 1.0f - origin.y) * deltaDist.y;
    }
    if (rayDir.z < 0) {
        step.z = -1;
        sideDist.z = (origin.z - currentBlockPos.z) * deltaDist.z;
    } else {
        step.z = 1;
        sideDist.z = (currentBlockPos.z + 1.0f - origin.z) * deltaDist.z;
    }
    
    float traveled = 0.0f;
    glm::ivec3 lastBlockPos = currentBlockPos;

    while (traveled < maxDistance) {
        if (sideDist.x < sideDist.y && sideDist.x < sideDist.z) {
            traveled = sideDist.x;
            sideDist.x += deltaDist.x;
            lastBlockPos = currentBlockPos;
            currentBlockPos.x += step.x;
        } else if (sideDist.y < sideDist.z) {
            traveled = sideDist.y;
            sideDist.y += deltaDist.y;
            lastBlockPos = currentBlockPos;
            currentBlockPos.y += step.y;
        } else {
            traveled = sideDist.z;
            sideDist.z += deltaDist.z;
            lastBlockPos = currentBlockPos;
            currentBlockPos.z += step.z;
        }

        if (world.getBlock(currentBlockPos.x, currentBlockPos.y, currentBlockPos.z) != BlockID::Air) {
            return RaycastResult{currentBlockPos, lastBlockPos};
        }
    }

    return std::nullopt; // No block was hit
}

} // namespace RaycastSystem