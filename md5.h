#pragma once

#include "api.h"
#include "digest.h"

namespace md5 {
    enum {
        hashbytes = 16,
        blockbytes = 64
    };
    enum {
        s11 = 7,
        s12 = 12,
        s13 = 17,
        s14 = 22,
        s21 = 5,
        s22 = 9,
        s23 = 14,
        s24 = 20,
        s31 = 4,
        s32 = 11,
        s33 = 16,
        s34 = 23,
        s41 = 6,
        s42 = 10,
        s43 = 15,
        s44 = 21
    };
}

#define MD5_F1(x, y, z)  (((x) & (y)) | ((~x) & (z)))
#define MD5_G1(x, y, z)  (((x) & (z)) | ((y) & (~z)))
#define MD5_H1(x, y, z)  ((x) ^ (y) ^ (z))
#define MD5_I1(x, y, z)  ((y) ^ ((x) | (~z)))

#define MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define MD5_FF(a, b, c, d, x, s, ac) { \
    (a) += MD5_F1 ((b), (c), (d)) + (x) + (unsigned int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); }

#define MD5_GG(a, b, c, d, x, s, ac) { \
    (a) += MD5_G1 ((b), (c), (d)) + (x) + (unsigned int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); }

#define MD5_HH(a, b, c, d, x, s, ac) { \
    (a) += MD5_H1 ((b), (c), (d)) + (x) + (unsigned int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); }

#define MD5_II(a, b, c, d, x, s, ac) { \
    (a) += MD5_I1 ((b), (c), (d)) + (x) + (unsigned int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); }

class MD5 : public Digest {
public:
    HASH32 _hash;

    EXPORT MD5();
    EXPORT ~MD5() override;
    EXPORT void Begin() override;
    EXPORT void Transform() override;
    EXPORT void End() override;
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
    EXPORT uint8_t* PutDigestInfo(uint8_t* buf) override;
};
