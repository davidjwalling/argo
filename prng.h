#pragma once

#include "api.h"
#include "des.h"
#include "random.h"

namespace prng {
    enum {
        recsize = cipher::recsize
    };
}

class PRNG {
public:
    uint8_t _v[prng::recsize];
    Random _random;
    DES3 _des3;

    EXPORT PRNG();
    EXPORT ~PRNG();
    EXPORT void Init();
    EXPORT void Reset();
    EXPORT void GetRandomBytes(uint8_t* out, size_t len);
};
