#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>

enum class BlockID : uint8_t {
    Air,
    Grass,
    Dirt,
    Stone
};

#endif