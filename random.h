#pragma once

#include "api.h"

class Random {
    void Init();
    void Seed();

    uint32_t _k;
    uint32_t _m;
    uint32_t _x;
    uint32_t _y;
    uint32_t _z;
    uint32_t _w;
    uint32_t _c;

public:
    void Reset();

    EXPORT Random();
    EXPORT ~Random();
    EXPORT uint32_t Rand();
    EXPORT uint32_t RandInRange(uint32_t lo, uint32_t hi);
    EXPORT void Fill(uint8_t* out, size_t bytes);
};

