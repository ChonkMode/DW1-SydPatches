#pragma once

#include "extern/dtl/types.hpp"

class Curling
{
public:
    static int32_t getWallZone(int16_t x, int16_t z);
};

extern "C"
{
    int32_t karGetWallZone(int16_t x, int16_t z);
}
