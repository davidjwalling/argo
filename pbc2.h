#pragma once

#include "api.h"
#include "hmac.h"
#include "prng.h"

namespace pbc2 {
    enum {
        maxdkbytes = 32
    };
}

class PBC2 : public HMAC {
public:
    size_t _passwordLen;
    size_t _saltLen;
    size_t _count;
    uint8_t* _password;
    uint8_t* _salt;
    uint8_t* _iv;
    PRNG _prng;

    EXPORT PBC2();
    EXPORT ~PBC2();
    EXPORT void Init();
    EXPORT void Reset();
    EXPORT void DeriveKey(uint8_t* key, size_t keyLen);
    EXPORT void Transform(uint8_t* out, size_t round);
    EXPORT void Encrypt(uint8_t* out, uint8_t* in, size_t* len);
    EXPORT void Decrypt(uint8_t* out, uint8_t* in, size_t* len);
    EXPORT void GenSalt(size_t len);
    EXPORT size_t GetCount();
    EXPORT uint8_t* GetIV(uint8_t* out);
    EXPORT uint8_t* GetSalt(uint8_t* out);
    EXPORT void SetCount(size_t count);
    EXPORT void SetIV(uint8_t* iv);
    EXPORT void SetPassword(uint8_t* password, size_t len);
    EXPORT void SetSalt(uint8_t* salt, size_t len);
};
