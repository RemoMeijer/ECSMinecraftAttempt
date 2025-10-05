#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "world/world.h"
#include "physics/aabb.h"
#include <glm/glm.hpp>

namespace PhysicsSystem {
    void resolveCollision(World& world, AABB& entityAABB, glm::vec3& position, glm::vec3& velocity, bool& onGround, float deltaTime);
}

#endif