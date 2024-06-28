#include "random.h"

Random::Random()
{
    Init();
    Seed();
}

Random::~Random()
{
    Reset();
}

void Random::Init()
{
    _k = 0;
    _m = 0;
    _x = 0;
    _y = 0;
    _z = 0;
    _w = 0;
    _c = 0;
}

void Random::Reset()
{
    Init();
}

void Random::Seed()
{
    uint32_t t = (uint32_t)time(0);
    t = ((t << 16) | (t >> 16));
    _k = clock();
    _k ^= ((_k << 11) | (_k >> 21)) ^ ((_k << 21) | (_k >> 11));
    SYSTEMTIME st;
    GetSystemTime(&st);
    _m = st.wMilliseconds;
    _m ^= ((_m << 10) | (_m >> 22)) ^ ((_m << 20) | (_m >> 12));
    uint32_t seed = _k ^ t ^ _m;
    _x = seed ^ 1;
    _y = seed ^ 2;
    _z = seed ^ 4;
    _w = seed ^ 8;
    _c = 0;
}

uint32_t Random::Rand()
{
    _k = (_z >> 2) + (_w >> 3) + (_c >> 2);
    _m = _w + _w + _z + _c;
    _z = _w;
    _w = _m;
    _c = _k >> 30;
    _x = _x * 69069 + 1;
    _y ^= (_y << 13) ^ (_y >> 17) ^ (_y << 5);
    return(_x + _y + _w);
}

uint32_t Random::RandInRange(uint32_t lo, uint32_t hi)
{
    if (hi <= lo)
        return lo;
    return(lo + (Rand() % (hi - lo)));
}

void Random::Fill(uint8_t* out, size_t bytes)
{
    while (bytes >= 4) {
        *((uint32_t*)out) = Rand();
        out += 4;
        bytes -= 4;
    }
    if (bytes) {
        uint32_t rnd = Rand();
        while (bytes) {
            *out = rnd & 255;
            rnd >>= 8;
            bytes--;
        }
    }
}