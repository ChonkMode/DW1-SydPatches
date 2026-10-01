#include "Curling.hpp"

int32_t Curling::getWallZone(int16_t x, int16_t z)
{
    const int32_t posX = x;
    const int32_t posZ = z;

    if (posZ >= 1125) return 6;

    if (posZ >= -2000) {
        if (posX < -724) return 1;
        if (posX >= 725) return 2;
    }
    else {
        const int32_t doubledX = posX * 2;

        // Both X conditions include 425; retail tests the first diagonal before the second.
        if (posZ + doubledX < -3524 && posX < 426) return 3;
        if (posZ - doubledX < -3524 && posX >= 425) return 4;
        if (posZ < -2524) return 5;
    }

    return 0;
}

extern "C"
{
    int32_t karGetWallZone(int16_t x, int16_t z)
    {
        return Curling::getWallZone(x, z);
    }
}
