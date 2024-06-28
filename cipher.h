#pragma once

#include "api.h"

namespace cipher {
    enum {
        keysize = 8,
        ivsize = 8,
        recsize = 8,
        blockrecs = 4096,
        blocksize = cipher::recsize * cipher::blockrecs
    };
}

class EXPORT Cipher {
public:
    int _err = 0;
    uint8_t _iv[cipher::ivsize] = { 0 };
    uint8_t _ivt[cipher::ivsize] = { 0 };
    uint8_t _key[cipher::keysize * 3] = { 0 };

    void SetIV();
    void GetKey(uint8_t* buf);

    virtual void SetKey(uint8_t* key) = 0;
    virtual size_t GetBlockBytes() = 0;
    virtual void Encrypt(uint8_t* plain, uint8_t* cipher) = 0;
    virtual void Decrypt(uint8_t* cipher, uint8_t* plain) = 0;
    virtual uint8_t* PutOID(uint8_t* buf) = 0;

    virtual void SetIV(uint8_t* iv);
};
