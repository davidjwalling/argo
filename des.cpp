#include "des.h"
#include "asn.h"
#include "oid.h"

static uint32_t sbox1[64] = {
    0x01010400, 0x00000000, 0x00010000, 0x01010404, 0x01010004, 0x00010404, 0x00000004, 0x00010000,
    0x00000400, 0x01010400, 0x01010404, 0x00000400, 0x01000404, 0x01010004, 0x01000000, 0x00000004,
    0x00000404, 0x01000400, 0x01000400, 0x00010400, 0x00010400, 0x01010000, 0x01010000, 0x01000404,
    0x00010004, 0x01000004, 0x01000004, 0x00010004, 0x00000000, 0x00000404, 0x00010404, 0x01000000,
    0x00010000, 0x01010404, 0x00000004, 0x01010000, 0x01010400, 0x01000000, 0x01000000, 0x00000400,
    0x01010004, 0x00010000, 0x00010400, 0x01000004, 0x00000400, 0x00000004, 0x01000404, 0x00010404,
    0x01010404, 0x00010004, 0x01010000, 0x01000404, 0x01000004, 0x00000404, 0x00010404, 0x01010400,
    0x00000404, 0x01000400, 0x01000400, 0x00000000, 0x00010004, 0x00010400, 0x00000000, 0x01010004 };

static uint32_t sbox2[64] = {
    0x80108020, 0x80008000, 0x00008000, 0x00108020, 0x00100000, 0x00000020, 0x80100020, 0x80008020,
    0x80000020, 0x80108020, 0x80108000, 0x80000000, 0x80008000, 0x00100000, 0x00000020, 0x80100020,
    0x00108000, 0x00100020, 0x80008020, 0x00000000, 0x80000000, 0x00008000, 0x00108020, 0x80100000,
    0x00100020, 0x80000020, 0x00000000, 0x00108000, 0x00008020, 0x80108000, 0x80100000, 0x00008020,
    0x00000000, 0x00108020, 0x80100020, 0x00100000, 0x80008020, 0x80100000, 0x80108000, 0x00008000,
    0x80100000, 0x80008000, 0x00000020, 0x80108020, 0x00108020, 0x00000020, 0x00008000, 0x80000000,
    0x00008020, 0x80108000, 0x00100000, 0x80000020, 0x00100020, 0x80008020, 0x80000020, 0x00100020,
    0x00108000, 0x00000000, 0x80008000, 0x00008020, 0x80000000, 0x80100020, 0x80108020, 0x00108000 };

static uint32_t sbox3[64] = {
    0x00000208, 0x08020200, 0x00000000, 0x08020008, 0x08000200, 0x00000000, 0x00020208, 0x08000200,
    0x00020008, 0x08000008, 0x08000008, 0x00020000, 0x08020208, 0x00020008, 0x08020000, 0x00000208,
    0x08000000, 0x00000008, 0x08020200, 0x00000200, 0x00020200, 0x08020000, 0x08020008, 0x00020208,
    0x08000208, 0x00020200, 0x00020000, 0x08000208, 0x00000008, 0x08020208, 0x00000200, 0x08000000,
    0x08020200, 0x08000000, 0x00020008, 0x00000208, 0x00020000, 0x08020200, 0x08000200, 0x00000000,
    0x00000200, 0x00020008, 0x08020208, 0x08000200, 0x08000008, 0x00000200, 0x00000000, 0x08020008,
    0x08000208, 0x00020000, 0x08000000, 0x08020208, 0x00000008, 0x00020208, 0x00020200, 0x08000008,
    0x08020000, 0x08000208, 0x00000208, 0x08020000, 0x00020208, 0x00000008, 0x08020008, 0x00020200 };

static uint32_t sbox4[64] = {
    0x00802001, 0x00002081, 0x00002081, 0x00000080, 0x00802080, 0x00800081, 0x00800001, 0x00002001,
    0x00000000, 0x00802000, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00800080, 0x00800001,
    0x00000001, 0x00002000, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002001, 0x00002080,
    0x00800081, 0x00000001, 0x00002080, 0x00800080, 0x00002000, 0x00802080, 0x00802081, 0x00000081,
    0x00800080, 0x00800001, 0x00802000, 0x00802081, 0x00000081, 0x00000000, 0x00000000, 0x00802000,
    0x00002080, 0x00800080, 0x00800081, 0x00000001, 0x00802001, 0x00002081, 0x00002081, 0x00000080,
    0x00802081, 0x00000081, 0x00000001, 0x00002000, 0x00800001, 0x00002001, 0x00802080, 0x00800081,
    0x00002001, 0x00002080, 0x00800000, 0x00802001, 0x00000080, 0x00800000, 0x00002000, 0x00802080 };

static uint32_t sbox5[64] = {
    0x00000100, 0x02080100, 0x02080000, 0x42000100, 0x00080000, 0x00000100, 0x40000000, 0x02080000,
    0x40080100, 0x00080000, 0x02000100, 0x40080100, 0x42000100, 0x42080000, 0x00080100, 0x40000000,
    0x02000000, 0x40080000, 0x40080000, 0x00000000, 0x40000100, 0x42080100, 0x42080100, 0x02000100,
    0x42080000, 0x40000100, 0x00000000, 0x42000000, 0x02080100, 0x02000000, 0x42000000, 0x00080100,
    0x00080000, 0x42000100, 0x00000100, 0x02000000, 0x40000000, 0x02080000, 0x42000100, 0x40080100,
    0x02000100, 0x40000000, 0x42080000, 0x02080100, 0x40080100, 0x00000100, 0x02000000, 0x42080000,
    0x42080100, 0x00080100, 0x42000000, 0x42080100, 0x02080000, 0x00000000, 0x40080000, 0x42000000,
    0x00080100, 0x02000100, 0x40000100, 0x00080000, 0x00000000, 0x40080000, 0x02080100, 0x40000100 };

static uint32_t sbox6[64] = {
    0x20000010, 0x20400000, 0x00004000, 0x20404010, 0x20400000, 0x00000010, 0x20404010, 0x00400000,
    0x20004000, 0x00404010, 0x00400000, 0x20000010, 0x00400010, 0x20004000, 0x20000000, 0x00004010,
    0x00000000, 0x00400010, 0x20004010, 0x00004000, 0x00404000, 0x20004010, 0x00000010, 0x20400010,
    0x20400010, 0x00000000, 0x00404010, 0x20404000, 0x00004010, 0x00404000, 0x20404000, 0x20000000,
    0x20004000, 0x00000010, 0x20400010, 0x00404000, 0x20404010, 0x00400000, 0x00004010, 0x20000010,
    0x00400000, 0x20004000, 0x20000000, 0x00004010, 0x20000010, 0x20404010, 0x00404000, 0x20400000,
    0x00404010, 0x20404000, 0x00000000, 0x20400010, 0x00000010, 0x00004000, 0x20400000, 0x00404010,
    0x00004000, 0x00400010, 0x20004010, 0x00000000, 0x20404000, 0x20000000, 0x00400010, 0x20004010 };

static uint32_t sbox7[64] = {
    0x00200000, 0x04200002, 0x04000802, 0x00000000, 0x00000800, 0x04000802, 0x00200802, 0x04200800,
    0x04200802, 0x00200000, 0x00000000, 0x04000002, 0x00000002, 0x04000000, 0x04200002, 0x00000802,
    0x04000800, 0x00200802, 0x00200002, 0x04000800, 0x04000002, 0x04200000, 0x04200800, 0x00200002,
    0x04200000, 0x00000800, 0x00000802, 0x04200802, 0x00200800, 0x00000002, 0x04000000, 0x00200800,
    0x04000000, 0x00200800, 0x00200000, 0x04000802, 0x04000802, 0x04200002, 0x04200002, 0x00000002,
    0x00200002, 0x04000000, 0x04000800, 0x00200000, 0x04200800, 0x00000802, 0x00200802, 0x04200800,
    0x00000802, 0x04000002, 0x04200802, 0x04200000, 0x00200800, 0x00000000, 0x00000002, 0x04200802,
    0x00000000, 0x00200802, 0x04200000, 0x00000800, 0x04000002, 0x04000800, 0x00000800, 0x00200002 };

static uint32_t sbox8[64] = {
    0x10001040, 0x00001000, 0x00040000, 0x10041040, 0x10000000, 0x10001040, 0x00000040, 0x10000000,
    0x00040040, 0x10040000, 0x10041040, 0x00041000, 0x10041000, 0x00041040, 0x00001000, 0x00000040,
    0x10040000, 0x10000040, 0x10001000, 0x00001040, 0x00041000, 0x00040040, 0x10040040, 0x10041000,
    0x00001040, 0x00000000, 0x00000000, 0x10040040, 0x10000040, 0x10001000, 0x00041040, 0x00040000,
    0x00041040, 0x00040000, 0x10041000, 0x00001000, 0x00000040, 0x10040040, 0x00001000, 0x00041040,
    0x10001000, 0x00000040, 0x10000040, 0x10040000, 0x10040040, 0x10000000, 0x00040000, 0x10001040,
    0x00000000, 0x10041040, 0x00040040, 0x10000040, 0x10040000, 0x10001000, 0x10001040, 0x00000000,
    0x10041040, 0x00041000, 0x00041000, 0x00001040, 0x00001040, 0x00040040, 0x10000000, 0x10041000 };

static uint32_t leftkey_swap[16] = {
    0x00000000, 0x00000001, 0x00000100, 0x00000101,
    0x00010000, 0x00010001, 0x00010100, 0x00010101,
    0x01000000, 0x01000001, 0x01000100, 0x01000101,
    0x01010000, 0x01010001, 0x01010100, 0x01010101 };

static uint32_t rightkey_swap[16] = {
    0x00000000, 0x01000000, 0x00010000, 0x01010000, 0x00000100, 0x01000100, 0x00010100, 0x01010100,
    0x00000001, 0x01000001, 0x00010001, 0x01010001, 0x00000101, 0x01000101, 0x00010101, 0x01010101 };

static uint8_t encrypt_rotate_tab[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

static const uint8_t odd_parity[256] = {
    1, 1, 2, 2, 4, 4, 7, 7, 8, 8, 11, 11, 13, 13, 14, 14,
    16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
    32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
    49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
    64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
    81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
    97, 97, 98, 98, 100, 100, 103, 103, 104, 104, 107, 107, 109, 109, 110, 110,
    112, 112, 115, 115, 117, 117, 118, 118, 121, 121, 122, 122, 124, 124, 127, 127,
    128, 128, 131, 131, 133, 133, 134, 134, 137, 137, 138, 138, 140, 140, 143, 143,
    145, 145, 146, 146, 148, 148, 151, 151, 152, 152, 155, 155, 157, 157, 158, 158,
    161, 161, 162, 162, 164, 164, 167, 167, 168, 168, 171, 171, 173, 173, 174, 174,
    176, 176, 179, 179, 181, 181, 182, 182, 185, 185, 186, 186, 188, 188, 191, 191,
    193, 193, 194, 194, 196, 196, 199, 199, 200, 200, 203, 203, 205, 205, 206, 206,
    208, 208, 211, 211, 213, 213, 214, 214, 217, 217, 218, 218, 220, 220, 223, 223,
    224, 224, 227, 227, 229, 229, 230, 230, 233, 233, 234, 234, 236, 236, 239, 239,
    241, 241, 242, 242, 244, 244, 247, 247, 248, 248, 251, 251, 253, 253, 254, 254 };

void DES::InitialPermutation()
{
    INITIAL_PERMUTATION(_left, _work, _right)
}

void DES::FinalPermutation()
{
    FINAL_PERMUTATION(_right, _work, _left)
}

void DES::RRound()
{
    DES_ROUND(_right, _left, _work, _keys)
}

void DES::LRound()
{
    DES_ROUND(_left, _right, _work, _keys)
}

void DES::KeySchedule(uint8_t* raw, uint32_t* sub)
{
    READ_64BIT_DATA(raw, _left, _right);
    DO_PERMUTATION(_right, _work, _left, 4, 0x0f0f0f0f);
    DO_PERMUTATION(_right, _work, _left, 0, 0x10101010);

    _left = (leftkey_swap[(_left >> 0) & 0xf] << 3) | (leftkey_swap[(_left >> 8) & 0xf] << 2) | (leftkey_swap[(_left >> 16) & 0xf] << 1) | (leftkey_swap[(_left >> 24) & 0xf]) | (leftkey_swap[(_left >> 5) & 0xf] << 7) | (leftkey_swap[(_left >> 13) & 0xf] << 6) | (leftkey_swap[(_left >> 21) & 0xf] << 5) | (leftkey_swap[(_left >> 29) & 0xf] << 4);
    _left &= 0x0fffffff;
    _right = (rightkey_swap[(_right >> 1) & 0xf] << 3) | (rightkey_swap[(_right >> 9) & 0xf] << 2) | (rightkey_swap[(_right >> 17) & 0xf] << 1) | (rightkey_swap[(_right >> 25) & 0xf]) | (rightkey_swap[(_right >> 4) & 0xf] << 7) | (rightkey_swap[(_right >> 12) & 0xf] << 6) | (rightkey_swap[(_right >> 20) & 0xf] << 5) | (rightkey_swap[(_right >> 28) & 0xf] << 4);
    _right &= 0x0fffffff;

    for (int round = 0; round < 16; round++) {
        _left = ((_left << encrypt_rotate_tab[round]) | (_left >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
        _right = ((_right << encrypt_rotate_tab[round]) | (_right >> (28 - encrypt_rotate_tab[round]))) & 0x0fffffff;
        *sub++ = ((_left << 4) & 0x24000000) | ((_left << 28) & 0x10000000) | ((_left << 14) & 0x08000000) | ((_left << 18) & 0x02080000) | ((_left << 6) & 0x01000000) | ((_left << 9) & 0x00200000) | ((_left >> 1) & 0x00100000) | ((_left << 10) & 0x00040000) | ((_left << 2) & 0x00020000) | ((_left >> 10) & 0x00010000) | ((_right >> 13) & 0x00002000) | ((_right >> 4) & 0x00001000) | ((_right << 6) & 0x00000800) | ((_right >> 1) & 0x00000400) | ((_right >> 14) & 0x00000200) | ((_right) & 0x00000100) | ((_right >> 5) & 0x00000020) | ((_right >> 10) & 0x00000010) | ((_right >> 3) & 0x00000008) | ((_right >> 18) & 0x00000004) | ((_right >> 26) & 0x00000002) | ((_right >> 24) & 0x00000001);
        *sub++ = ((_left << 15) & 0x20000000) | ((_left << 17) & 0x10000000) | ((_left << 10) & 0x08000000) | ((_left << 22) & 0x04000000) | ((_left >> 2) & 0x02000000) | ((_left << 1) & 0x01000000) | ((_left << 16) & 0x00200000) | ((_left << 11) & 0x00100000) | ((_left << 3) & 0x00080000) | ((_left >> 6) & 0x00040000) | ((_left << 15) & 0x00020000) | ((_left >> 4) & 0x00010000) | ((_right >> 2) & 0x00002000) | ((_right << 8) & 0x00001000) | ((_right >> 14) & 0x00000808) | ((_right >> 9) & 0x00000400) | ((_right) & 0x00000200) | ((_right << 7) & 0x00000100) | ((_right >> 7) & 0x00000020) | ((_right >> 3) & 0x00000011) | ((_right << 2) & 0x00000004) | ((_right >> 21) & 0x00000002);
    }
}

void DES::Encipher()
{
    InitialPermutation();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    FinalPermutation();
}

void DES::SetKey(uint8_t* val)
{
    KeySchedule(val, _encrypt_subkeys);
    for (int i = 0; i < 32; i += 2) {
        _decrypt_subkeys[i] = _encrypt_subkeys[30 - i];
        _decrypt_subkeys[i + 1] = _encrypt_subkeys[31 - i];
    }
}

size_t DES::GetBlockBytes()
{
    return cipher::recsize;
}

void DES::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(plain, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DES::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _decrypt_subkeys;
    READ_64BIT_DATA(cipher, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(plain, _right, _left);
}

uint8_t* DES::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    *p++ = asn::sequence;
    *p++ = 9;
    *p++ = asn::oid;
    *p++ = 5;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgoiw;
    *p++ = oid::isoorgoiw_sec;
    *p++ = oid::isoorgoiw_secalg;
    *p++ = oid::isoorgoiw_secalg_desecb;
    *p++ = asn::null;
    *p++ = 0;
    return p;
}

void DESCBC::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _leftvector, _rightvector);
    READ_64BIT_DATA(plain, _left, _right);
    _left ^= _leftvector;
    _right ^= _rightvector;
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DESCBC::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _decrypt_subkeys;
    READ_64BIT_DATA(_ivt, _leftvector, _rightvector);
    READ_64BIT_DATA(cipher, _left, _right);
    WRITE_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
}

uint8_t* DESCBC::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 9 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 5;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgoiw;
    *p++ = oid::isoorgoiw_sec;
    *p++ = oid::isoorgoiw_secalg;
    *p++ = oid::isoorgoiw_secalg_descbc;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) {
        *p++ = *q++;
    }
    return p;
}

void DESCFB::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    READ_64BIT_DATA(plain, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(_ivt, _right, _left);
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DESCFB::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    READ_64BIT_DATA(cipher, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
    WRITE_64BIT_DATA(_ivt, _leftvector, _rightvector);
}

uint8_t* DESCFB::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 9 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 5;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgoiw;
    *p++ = oid::isoorgoiw_sec;
    *p++ = oid::isoorgoiw_secalg;
    *p++ = oid::isoorgoiw_secalg_descfb;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) {
        *p++ = *q++;
    };
    return p;
}

void DESOFB::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    READ_64BIT_DATA(plain, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DESOFB::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    READ_64BIT_DATA(cipher, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
}

uint8_t* DESOFB::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 9 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 5;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgoiw;
    *p++ = oid::isoorgoiw_sec;
    *p++ = oid::isoorgoiw_secalg;
    *p++ = oid::isoorgoiw_secalg_descfb;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) {
        *p++ = *q++;
    };
    return p;
}

void DES3::Set2Keys(uint8_t* key1, uint8_t* key2)
{
    KeySchedule(key1, _encrypt_subkeys);
    KeySchedule(key2, &_decrypt_subkeys[32]);
    for (int i = 0; i < 32; i += 2) {
        _decrypt_subkeys[i] = _encrypt_subkeys[30 - i];
        _decrypt_subkeys[i + 1] = _encrypt_subkeys[31 - i];
        _encrypt_subkeys[i + 32] = _decrypt_subkeys[62 - i];
        _encrypt_subkeys[i + 33] = _decrypt_subkeys[63 - i];
        _encrypt_subkeys[i + 64] = _encrypt_subkeys[i];
        _encrypt_subkeys[i + 65] = _encrypt_subkeys[i + 1];
        _decrypt_subkeys[i + 64] = _decrypt_subkeys[i];
        _decrypt_subkeys[i + 65] = _decrypt_subkeys[i + 1];
    }
}

void DES3::Set3Keys(uint8_t* key1, uint8_t* key2, uint8_t* key3)
{
    memcpy(&_key[0], key1, 8);
    memcpy(&_key[8], key2, 8);
    memcpy(&_key[16], key3, 8);
    for (int i = 0; i < 24; i++)
        _key[i] = odd_parity[_key[i]];
    KeySchedule(&_key[0], _encrypt_subkeys);
    KeySchedule(&_key[8], &_decrypt_subkeys[32]);
    KeySchedule(&_key[16], &_encrypt_subkeys[64]);
    for (int i = 0; i < 32; i += 2) {
        _decrypt_subkeys[i] = _encrypt_subkeys[94 - i];
        _decrypt_subkeys[i + 1] = _encrypt_subkeys[95 - i];
        _encrypt_subkeys[i + 32] = _decrypt_subkeys[62 - i];
        _encrypt_subkeys[i + 33] = _decrypt_subkeys[63 - i];
        _decrypt_subkeys[i + 64] = _encrypt_subkeys[30 - i];
        _decrypt_subkeys[i + 65] = _encrypt_subkeys[31 - i];
    }
}

void DES3::SetKey(uint8_t* key)
{
    Set3Keys(&key[0], &key[8], &key[16]);
}

void DES3::Encipher()
{
    InitialPermutation();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();

    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();

    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    RRound();
    LRound();
    FinalPermutation();
}

uint8_t* DES3::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    *p++ = asn::sequence;
    *p++ = 13;
    *p++ = asn::oid;
    *p++ = 5;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgdod;
    *p++ = oid::isoorgdod_int;
    *p++ = oid::isoorgdod_intprv;
    *p++ = oid::isoorgdod_intprv_iana;
    *p++ = OID_HI(oid::isoorgdod_intprv_ianalan);
    *p++ = OID_LO(oid::isoorgdod_intprv_ianalan);
    *p++ = oid::isoorgdod_intprv_ianalan_alg;
    *p++ = oid::isoorgdod_intprv_ianalan_alg3des;
    *p++ = asn::null;
    *p++ = 0;
    return p;
}

void DES3CBC::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _leftvector, _rightvector);
    READ_64BIT_DATA(plain, _left, _right);
    _left ^= _leftvector;
    _right ^= _rightvector;
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DES3CBC::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _decrypt_subkeys;
    READ_64BIT_DATA(_ivt, _leftvector, _rightvector);
    READ_64BIT_DATA(cipher, _left, _right);
    WRITE_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
}

uint8_t* DES3CBC::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 12 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 8;
    *p++ = OID_BYTE1(oid::iso, oid::isombr);
    *p++ = OID_HI(oid::isombrus);
    *p++ = OID_LO(oid::isombrus);
    *p++ = OID_HIHI(oid::isombrus_rsadsi);
    *p++ = OID_HI(oid::isombrus_rsadsi);
    *p++ = OID_LO(oid::isombrus_rsadsi);
    *p++ = oid::isombrus_rsadsi_enc;
    *p++ = oid::isombrus_rsadsi_encdes3cbc;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) { *p++ = *q++; }
    return p;
}

void DES3CFB::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    READ_64BIT_DATA(plain, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(_ivt, _right, _left);
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DES3CFB::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    READ_64BIT_DATA(cipher, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
    WRITE_64BIT_DATA(_ivt, _leftvector, _rightvector);
}

uint8_t* DES3CFB::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 13 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 9;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgdod;
    *p++ = oid::isoorgdod_int;
    *p++ = oid::isoorgdod_intprv;
    *p++ = oid::isoorgdod_intprv_iana;
    *p++ = OID_HI(oid::isoorgdod_intprv_ianalan);
    *p++ = OID_LO(oid::isoorgdod_intprv_ianalan);
    *p++ = oid::isoorgdod_intprv_ianalan_alg;
    *p++ = oid::isoorgdod_intprv_ianalan_alg3descfb;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) { *p++ = *q++; }
    return p;
}

void DES3OFB::Encrypt(uint8_t* plain, uint8_t* cipher)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    READ_64BIT_DATA(plain, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(cipher, _right, _left);
}

void DES3OFB::Decrypt(uint8_t* cipher, uint8_t* plain)
{
    _keys = _encrypt_subkeys;
    READ_64BIT_DATA(_ivt, _left, _right);
    Encipher();
    WRITE_64BIT_DATA(_ivt, _right, _left);
    READ_64BIT_DATA(cipher, _leftvector, _rightvector);
    _left ^= _rightvector;
    _right ^= _leftvector;
    WRITE_64BIT_DATA(plain, _right, _left);
}

uint8_t* DES3OFB::PutOID(uint8_t* buf)
{
    uint8_t* p = buf;
    uint8_t* q;
    size_t len;
    *p++ = asn::sequence;
    *p++ = 16 + cipher::ivsize;
    *p++ = asn::oid;
    *p++ = 9;
    *p++ = OID_BYTE1(oid::iso, oid::isoorg);
    *p++ = oid::isoorgdod;
    *p++ = oid::isoorgdod_int;
    *p++ = oid::isoorgdod_intprv;
    *p++ = oid::isoorgdod_intprv_iana;
    *p++ = OID_HI(oid::isoorgdod_intprv_ianalan);
    *p++ = OID_LO(oid::isoorgdod_intprv_ianalan);
    *p++ = oid::isoorgdod_intprv_ianalan_alg;
    *p++ = oid::isoorgdod_intprv_ianalan_alg3desofb;
    *p++ = asn::octetstring;
    *p++ = cipher::ivsize;
    for (q = _iv, len = cipher::ivsize; len; len--) { *p++ = *q++; }
    return p;
}
