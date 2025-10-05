#ifndef RAYCAST_H
#define RAYCAST_H

#include "world/world.h"
#include <glm/glm.hpp>
#include <optional>

struct RaycastResult {
    glm::ivec3 blockPosition;
    glm::ivec3 previousBlockPosition;
};

namespace RaycastSystem {
    std::optional<RaycastResult> cast(World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance);
}

#endif