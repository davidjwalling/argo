#pragma once

#include "api.h"
#include "cipher.h"

namespace aes {
    enum {
        blockbytes = 16,
        keybytes = 32,
        schedulebytes = 240
    };
    namespace mode {
        enum {
            _128 = 0b0000,
            _192 = 0b0001,
            _256 = 0b0010,
            ecb = 0b0000,
            cbc = 0b0100,
            cfb = 0b1000,
            ofb = 0b1100,
            _128ecb = aes::mode::_128 | aes::mode::ecb,
            _192ecb = aes::mode::_192 | aes::mode::ecb,
            _256ecb = aes::mode::_256 | aes::mode::ecb,
            _128cbc = aes::mode::_128 | aes::mode::cbc,
            _192cbc = aes::mode::_192 | aes::mode::cbc,
            _256cbc = aes::mode::_256 | aes::mode::cbc,
            _128cfb = aes::mode::_128 | aes::mode::cfb,
            _192cfb = aes::mode::_192 | aes::mode::cfb,
            _256cfb = aes::mode::_256 | aes::mode::cfb,
            _128ofb = aes::mode::_128 | aes::mode::ofb,
            _192ofb = aes::mode::_192 | aes::mode::ofb,
            _256ofb = aes::mode::_256 | aes::mode::ofb
        };
    }
}

class AES : public Cipher {
public:
    uint32_t _keyBytes;
    uint32_t _rounds;
    uint32_t _scheduleBytes;
    uint32_t _mode;

    uint8_t _key[aes::keybytes];
    uint8_t _iv[aes::blockbytes];
    uint8_t _input[aes::blockbytes];
    uint8_t _state[aes::blockbytes];
    uint8_t _output[aes::blockbytes];
    uint8_t _schedule[aes::schedulebytes];

    void Init();
    void Reset();
    void ExpandKey();
    void SetInput(uint8_t* input);
    void GetOutput(uint8_t* output);
    void Encipher();
    void InvCipher();

    EXPORT AES();
    EXPORT ~AES();
    EXPORT size_t GetBlockBytes() { return aes::blockbytes; }
    EXPORT void SetMode(uint32_t mode);
    EXPORT void SetKey(uint8_t* key);
    EXPORT void SetIV(uint8_t* iv);
    EXPORT virtual void Encrypt(uint8_t* plain, uint8_t* cipher) = 0;
    EXPORT virtual void Decrypt(uint8_t* cipher, uint8_t* plain) = 0;
};

class AESECB : public AES {
public:
    EXPORT void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    EXPORT void Decrypt(uint8_t* cipher, uint8_t* plain) override;
};

class AESCBC : public AES {
public:
    EXPORT void Encrypt(uint8_t* plain, uint8_t* cipher) override;
    EXPORT void Decrypt(uint8_t* cipher, uint8_t* plain) override;
};

class AES128ECB : public AESECB {
public:
    EXPORT AES128ECB();
    EXPORT ~AES128ECB();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};

class AES128CBC : public AESCBC {
public:
    EXPORT AES128CBC();
    EXPORT ~AES128CBC();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};

class AES192ECB : public AESECB {
public:
    EXPORT AES192ECB();
    EXPORT ~AES192ECB();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};

class AES192CBC : public AESCBC {
public:
    EXPORT AES192CBC();
    EXPORT ~AES192CBC();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};

class AES256ECB : public AESECB {
public:
    EXPORT AES256ECB();
    EXPORT ~AES256ECB();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};

class AES256CBC : public AESCBC {
public:
    EXPORT AES256CBC();
    EXPORT ~AES256CBC();
    EXPORT uint8_t* PutOID(uint8_t* buf) override;
};
