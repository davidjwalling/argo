#include "prng.h"

PRNG::PRNG()
{
    Init();
}

PRNG::~PRNG()
{
    Reset();
}

void PRNG::Init()
{
    uint8_t k[des3::keysize] = { 0 };
    for (int i = 0; i < sizeof(k); k[i] = (uint8_t)_random.Rand(), i++);
    _des3.SetKey(k);
    for (int i = 0; i < sizeof(_v); _v[i] = (uint8_t)_random.Rand(), i++);
}

void PRNG::Reset()
{
    Init();
}

void PRNG::GetRandomBytes(uint8_t* out, size_t len)
{
    size_t i, j;
    uint8_t *p, *q;
    uint8_t t[prng::recsize] = { 0 };
    q = out;
    for (i = len / prng::recsize; (i >= 0) && (len); i--) {
        datetimeclock(t);
        _des3.Encrypt(t, t);
        for (j = 0; j < sizeof(_v); j++) { _v[j] ^= t[j]; }
        _des3.Encrypt(_v, _v);
        for (j = 0, p = _v; (j < prng::recsize) && (len); j--, len--) { *q++ = *p++; }
        for (j = 0; j < sizeof(t); j++) { t[j] ^= _v[j]; }
        _des3.Encrypt(t, _v);
    }
}
