#pragma once

#include "api.h"
#include "sha.h"

namespace hmac {
    enum {
        maxblockbytes = sha512::blockbytes,
        maxhashbytes = sha512::hashbytes,
        innerpad = 0x36,
        outerpad = 0x5c
    };
    namespace alg {
        enum {
            md5 = 0,
            sha1,
            sha256,
            sha224,
            sha512,
            sha384,
            default_ = hmac::alg::md5
        };
    }
}

class HMAC {
public:
    size_t _blockBytes;
    Digest* _digest;
    uint8_t _k[hmac::maxblockbytes];

    EXPORT HMAC();
    EXPORT ~HMAC();
    EXPORT void Init();
    EXPORT void Reset();
    EXPORT bool SetDigestAlg(size_t algId);
    EXPORT void SetKey(uint8_t* key, size_t len);
    EXPORT void Begin();
    EXPORT void Update(uint8_t* in, size_t len);
    EXPORT void End();
    EXPORT void GetMAC(uint8_t* out, size_t len);
};
