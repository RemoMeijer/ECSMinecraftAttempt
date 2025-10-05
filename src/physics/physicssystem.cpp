// src/physics/physicssystem.cpp
#include "physics/physicssystem.h"
#include <algorithm>
#include <cmath>

void PhysicsSystem::resolveCollision(World& world, AABB& entityAABB, glm::vec3& position, glm::vec3& velocity, bool& onGround, float deltaTime) {
    // A small buffer to prevent floating-point errors from causing sticking.
    const float SKIN_WIDTH = 0.005f;
    onGround = false;

    glm::vec3 entitySize = entityAABB.max - entityAABB.min;
    
    // Y-AXIS
    position.y += velocity.y * deltaTime;
    entityAABB = AABB(position, entitySize);
    int minY = floor(entityAABB.min.y), maxY = ceil(entityAABB.max.y);
    int minX = floor(entityAABB.min.x), maxX = ceil(entityAABB.max.x);
    int minZ = floor(entityAABB.min.z), maxZ = ceil(entityAABB.max.z);

    for (int y = minY; y < maxY; ++y) {
    for (int x = minX; x < maxX; ++x) {
    for (int z = minZ; z < maxZ; ++z) {
        if (world.getBlock(x, y, z) != BlockID::Air) {
            AABB blockAABB({x + 0.5f, y + 0.5f, z + 0.5f}, glm::vec3(1.0f));
            if (entityAABB.intersects(blockAABB)) {
                if (velocity.y > 0) { // Moving up
                    position.y = blockAABB.min.y - entitySize.y / 2.0f - SKIN_WIDTH;
                } else if (velocity.y < 0) { // Moving down
                    position.y = blockAABB.max.y + entitySize.y / 2.0f + SKIN_WIDTH;
                    onGround = true;
                }
                velocity.y = 0;
                entityAABB = AABB(position, entitySize);
            }
        }
    }}}

    // X-AXIS
    position.x += velocity.x * deltaTime;
    entityAABB = AABB(position, entitySize);
    minX = floor(entityAABB.min.x); maxX = ceil(entityAABB.max.x);
    minY = floor(entityAABB.min.y); maxY = ceil(entityAABB.max.y);
    minZ = floor(entityAABB.min.z); maxZ = ceil(entityAABB.max.z);

    for (int y = minY; y < maxY; ++y) {
    for (int x = minX; x < maxX; ++x) {
    for (int z = minZ; z < maxZ; ++z) {
        if (world.getBlock(x, y, z) != BlockID::Air) {
            AABB blockAABB({x + 0.5f, y + 0.5f, z + 0.5f}, glm::vec3(1.0f));
            if (entityAABB.intersects(blockAABB)) {
                if (velocity.x > 0) { // Moving right
                    position.x = blockAABB.min.x - entitySize.x / 2.0f - SKIN_WIDTH;
                } else if (velocity.x < 0) { // Moving left
                    position.x = blockAABB.max.x + entitySize.x / 2.0f + SKIN_WIDTH;
                }
                velocity.x = 0;
                entityAABB = AABB(position, entitySize);
            }
        }
    }}}
    
    // Z-AXIS
    position.z += velocity.z * deltaTime;
    entityAABB = AABB(position, entitySize);
    minZ = floor(entityAABB.min.z); maxZ = ceil(entityAABB.max.z);
    minX = floor(entityAABB.min.x); maxX = ceil(entityAABB.max.x);
    minY = floor(entityAABB.min.y); maxY = ceil(entityAABB.max.y);

    for (int y = minY; y < maxY; ++y) {
    for (int x = minX; x < maxX; ++x) {
    for (int z = minZ; z < maxZ; ++z) {
        if (world.getBlock(x, y, z) != BlockID::Air) {
            AABB blockAABB({x + 0.5f, y + 0.5f, z + 0.5f}, glm::vec3(1.0f));
            if (entityAABB.intersects(blockAABB)) {
                if (velocity.z > 0) { // Moving forward
                    position.z = blockAABB.min.z - entitySize.z / 2.0f - SKIN_WIDTH;
                } else if (velocity.z < 0) { // Moving backward
                    position.z = blockAABB.max.z + entitySize.z / 2.0f + SKIN_WIDTH;
                }
                velocity.z = 0;
                entityAABB = AABB(position, entitySize);
            }
        }
    }}}
}