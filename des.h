#pragma once

#include "api.h"
#include "cipher.h"

namespace des {
    enum {
        keysize = 8,
        recsize = cipher::recsize
    };
}
namespace des3 {
    enum {
        keysize = 24,
        recsize = cipher::recsize
    };
}

#define DO_PERMUTATION(a, temp, b, offset, mask) \
    temp = ((a >> offset) ^ b) & mask; \
    b ^= temp; \
    a ^= temp << offset;

#define INITIAL_PERMUTATION(left, temp, right) \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f) \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff) \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333) \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff) \
    right = (right << 1) | (right >> 31); \
    temp = (left ^ right) & 0xaaaaaaaa; \
    right ^= temp; \
    left ^= temp; \
    left = (left << 1) | (left >> 31);

#define FINAL_PERMUTATION(left, temp, right) \
    left = (left << 31) | (left >> 1); \
    temp = (left ^ right) & 0xaaaaaaaa; \
    left ^= temp; \
    right ^= temp; \
    right = (right << 31) | (right >> 1); \
    DO_PERMUTATION(right, temp, left, 8, 0x00ff00ff) \
    DO_PERMUTATION(right, temp, left, 2, 0x33333333) \
    DO_PERMUTATION(left, temp, right, 16, 0x0000ffff) \
    DO_PERMUTATION(left, temp, right, 4, 0x0f0f0f0f)

#define DES_ROUND(from, to, work, subkey) \
    work = from ^ *subkey++; \
    to ^= sbox8[work & 0x3f]; \
    to ^= sbox6[(work >> 8) & 0x3f]; \
    to ^= sbox4[(work >> 16) & 0x3f]; \
    to ^= sbox2[(work >> 24) & 0x3f]; \
    work = ((from << 28) | (from >> 4)) ^ *subkey++; \
    to ^= sbox7[work & 0x3f]; \
    to ^= sbox5[(work >> 8) & 0x3f]; \
    to ^= sbox3[(work >> 16) & 0x3f]; \
    to ^= sbox1[(work >> 24) & 0x3f];

#define READ_64BIT_DATA(D, L, R) ( \
    (L = (D[0] << 24) | (D[1] << 16) | (D[2] << 8) | D[3]), \
    (R = (D[4] << 24) | (D[5] << 16) | (D[6] << 8) | D[7]))

#define WRITE_64BIT_DATA(D, L, R) ( \
    (D[0] = (uint8_t)((L >> 24) & 0xff)), \
    (D[1] = (uint8_t)((L >> 16) & 0xff)), \
    (D[2] = (uint8_t)((L >> 8) & 0xff)), \
    (D[3] = (uint8_t)((L)&0xff)), \
    (D[4] = (uint8_t)((R >> 24) & 0xff)), \
    (D[5] = (uint8_t)((R >> 16) & 0xff)), \
    (D[6] = (uint8_t)((R >> 8) & 0xff)), \
    (D[7] = (uint8_t)((R)&0xff)))

class EXPORT DES : public Cipher {
public:
    uint32_t _left = 0;
    uint32_t _right = 0;
    uint32_t _leftvector = 0;
    uint32_t _rightvector = 0;
    uint32_t _work = 0;
    uint32_t* _keys = nullptr;
    uint32_t _encrypt_subkeys[96] = { 0 };
    uint32_t _decrypt_subkeys[96] = { 0 };

    void InitialPermutation();
    void FinalPermutation();
    void RRound();
    void LRound();
    void KeySchedule(uint8_t* raw, uint32_t* sub);

    virtual void SetKey(uint8_t* val) override;
    virtual size_t GetBlockBytes() override;
    virtual void Encipher();
    virtual void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    virtual void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    virtual uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DESCBC : public DES {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DESCFB : public DES {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DESOFB : public DES {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DES3 : public DES {
public:
    void Set2Keys(uint8_t* key1, uint8_t* key2);
    void Set3Keys(uint8_t* key1, uint8_t* key2, uint8_t* key3);
    void SetKey(uint8_t* key) override;
    void Encipher() override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DES3CBC : public DES3 {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DES3CFB : public DES3 {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};

class EXPORT DES3OFB : public DES3 {
public:
    void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    void Decrypt(uint8_t* cipher, uint8_t* plain) override;
    uint8_t* PutOID(uint8_t* buf) override;
};
