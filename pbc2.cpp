#include "pbc2.h"

PBC2::PBC2()
{
    Init();
}

PBC2::~PBC2()
{
    Reset();
}

void PBC2::Init()
{
    _passwordLen = _saltLen = _count = 0;
    _password = _salt = _iv = nullptr;
}

void PBC2::Reset()
{
    freeptr(&_password);
    freeptr(&_salt);
    freeptr(&_iv);
    Init();
}

void PBC2::DeriveKey(uint8_t* key, size_t keyLen)
{
    size_t h, l, r, j;
    uint8_t* q;
    uint8_t t[hmac::maxhashbytes];
    SetDigestAlg(hmac::alg::sha1);
    SetKey(_password, _passwordLen);
    h = _digest->_hashBytes;
    l = keyLen / h;
    r = keyLen % h;
    //if (r)
    //    l++;
    for (j = 1, q = key; j <= l; j++, q += h) {
        Transform(q, j);
    }
    if (r) {
        Transform(t, j);
        memcpy(q, t, r);
    }
}

void PBC2::Transform(uint8_t* out, size_t round)
{
    size_t h, j, k;
    uint8_t cround[4];
    uint8_t cmac[hmac::maxhashbytes];
    uint8_t ct[hmac::maxhashbytes];
    h = _digest->_hashBytes;
    cround[0] = (uint8_t)((round >> 24) & 255);
    cround[1] = (uint8_t)((round >> 16) & 255);
    cround[2] = (uint8_t)((round >> 8) & 255);
    cround[3] = (uint8_t)(round & 255);
    Begin();
    Update(_salt, _saltLen);
    Update(cround, sizeof(cround));
    End();
    GetMAC(ct, h);
    memcpy(cmac, ct, h);
    for (j = 1; j < _count; j++) {
        Begin();
        Update(ct, h);
        End();
        GetMAC(ct, h);
        for (k = 0; k < h; k++) { cmac[k] ^= ct[k]; }
    }
    memcpy(out, cmac, h);
};

void PBC2::Encrypt(uint8_t* out, uint8_t* in, size_t* len)
{
    size_t b, h, j, l, r;
    uint8_t* p, * q;
    uint8_t ch;
    uint8_t dk[pbc2::maxdkbytes];
    uint8_t pad[cipher::recsize];
    uint8_t border1[16];
    DES3CBC des3cbc;
    uint8_t border2[16];
    memset(border1, '-', 16);
    memset(border2, '-', 16);
    DeriveKey(dk, sizeof(dk));
    des3cbc.SetKey(&dk[0]);
    des3cbc.SetIV(&dk[24]);
    setptr(&_iv, &dk[24], cipher::ivsize);
    b = *len;
    h = cipher::recsize;
    l = b / h;
    r = b % h;
    p = in;
    q = out;
    for (j = 0; j < l; j++) {
        des3cbc.Encrypt(p, q);
        p += cipher::recsize;
        q += cipher::recsize;
    }
    ch = (uint8_t)(cipher::recsize - r);
    memset(pad, ch, sizeof(pad));
    if (r)
        memcpy(pad, p, r);
    des3cbc.Encrypt(pad, q);
    q += cipher::recsize;
    *len = q - out;
}

void PBC2::Decrypt(uint8_t* out, uint8_t* in, size_t* len)
{
    size_t b, h, j, l;
    uint8_t* p, * q;
    uint8_t ch;
    uint8_t dk[pbc2::maxdkbytes];
    DES3CBC des3cbc;
    DeriveKey(dk, sizeof(dk));
    des3cbc.SetKey(&dk[0]);
    if (_iv)
        des3cbc.SetIV(_iv);
    else
        des3cbc.SetIV(&dk[24]);
    b = *len;
    h = cipher::recsize;
    l = b / h;
    p = in;
    q = out;
    for (j = 0; j < l; j++) {
        des3cbc.Decrypt(p, q);
        p += cipher::recsize;
        q += cipher::recsize;
    }
    ch = out[b - 1];
    q -= ch;
    *len = q - out;
}

void PBC2::GenSalt(size_t len)
{
    setptr(&_salt, 0, len);
    _prng.GetRandomBytes(_salt, len);
    _saltLen = len;
}

size_t PBC2::GetCount()
{
    return _count;
}

uint8_t* PBC2::GetIV(uint8_t* out)
{
    if (_iv)
        memcpy(out, _iv, cipher::ivsize);
    else
        memset(out, 0, cipher::ivsize);
    return out + cipher::ivsize;
}

uint8_t* PBC2::GetSalt(uint8_t* out)
{
    memcpy(out, _salt, _saltLen);
    return out + _saltLen;
}

void PBC2::SetCount(size_t count)
{
    _count = count;
}

void PBC2::SetIV(uint8_t* iv)
{
    setptr(&_iv, iv, cipher::ivsize);
}

void PBC2::SetPassword(uint8_t* password, size_t len)
{
    setptr(&_password, password, len);
    _passwordLen = len;
}

void PBC2::SetSalt(uint8_t* salt, size_t len)
{
    setptr(&_salt, salt, len);
    _saltLen = len;
}
