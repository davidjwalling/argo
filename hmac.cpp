#include "hmac.h"
#include "md5.h"
#include "sha.h"

HMAC::HMAC()
{
    Init();
}

HMAC::~HMAC()
{
    Reset();
}

void HMAC::Init()
{
    _blockBytes = 0;
    _digest = nullptr;
    memset(_k, 0, sizeof(_k));
}

void HMAC::Reset()
{
    if (_digest)
        delete _digest;
    Init();
}

bool HMAC::SetDigestAlg(size_t alg)
{
    switch (alg) {
    case hmac::alg::md5:
        Reset();
        _digest = new MD5;
        _blockBytes = md5::blockbytes;
        break;
    case hmac::alg::sha1:
        Reset();
        _digest = new SHA1;
        _blockBytes = sha1::blockbytes;
        break;
    case hmac::alg::sha256:
        Reset();
        _digest = new SHA256;
        _blockBytes = sha256::blockbytes;
        break;
    case hmac::alg::sha224:
        Reset();
        _digest = new SHA224;
        _blockBytes = sha224::blockbytes;
        break;
    case hmac::alg::sha512:
        Reset();
        _digest = new SHA512;
        _blockBytes = sha512::blockbytes;
        break;
    case hmac::alg::sha384:
        Reset();
        _digest = new SHA384;
        _blockBytes = sha384::blockbytes;
        break;
    default:
        return false;
    }
    return true;
}

void HMAC::SetKey(uint8_t* key, size_t len)
{
    memset(_k, 0, sizeof(_k));
    if (len > _blockBytes) {
        _digest->HashBuf(key, len);
        _digest->GetDigest(_k);
    } else {
        memcpy(_k, key, len);
    }
}

void HMAC::Begin()
{
    size_t j;
    uint8_t uc[1];
    _digest->Begin();
    for (j = 0; j < _blockBytes; j++) {
        uc[0] = (uint8_t)(_k[j] ^ hmac::innerpad);
        _digest->Update(uc, 1);
    }
}

void HMAC::Update(uint8_t* in, size_t len)
{
    _digest->Update(in, len);
}

void HMAC::End()
{
    size_t j;
    unsigned char uc[1];
    unsigned char innerHash[hmac::maxhashbytes];
    _digest->End();
    _digest->GetDigest(innerHash);
    _digest->Begin();
    for (j = 0; j < _blockBytes; j++) {
        uc[0] = (uint8_t)(_k[j] ^ hmac::outerpad);
        _digest->Update(uc, 1);
    }
    _digest->Update(innerHash, _digest->_hashBytes);
    _digest->End();
}

void HMAC::GetMAC(uint8_t* out, size_t len)
{
    memset(out, 0, len);
    _digest->GetDigest(out);
}
