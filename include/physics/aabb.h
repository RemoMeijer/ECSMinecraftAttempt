#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB(const glm::vec3& center, const glm::vec3& size) {
        min = center - size / 2.0f;
        max = center + size / 2.0f;
    }

    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

#endif