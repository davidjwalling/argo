#include "md5.h"
#include "asn.h"
#include "oid.h"

MD5::MD5()
{
    Begin();
}

MD5::~MD5()
{
    Reset();
}

void MD5::Begin()
{
    _blocks = _len = 0;
    _hashBytes = md5::hashbytes;
    _blockBytes = _remain = md5::blockbytes;
    _first = _next = _hash._m;
    _digest = _hash._d;
    memset(_hash._w, 0, sizeof(_hash._w));
    memset(_hash._d, 0, sizeof(_hash._d));
    _hash._h[3] = 0x67452301;
    _hash._h[2] = 0xefcdab89;
    _hash._h[1] = 0x98badcfe;
    _hash._h[0] = 0x10325476;
}

void MD5::Transform()
{
    uint32_t a, b, c, d;
    a = _hash._h[3];
    b = _hash._h[2];
    c = _hash._h[1];
    d = _hash._h[0];

    MD5_FF(a, b, c, d, _hash._w[0], md5::s11, 0xd76aa478); // 1
    MD5_FF(d, a, b, c, _hash._w[1], md5::s12, 0xe8c7b756); // 2
    MD5_FF(c, d, a, b, _hash._w[2], md5::s13, 0x242070db); // 3
    MD5_FF(b, c, d, a, _hash._w[3], md5::s14, 0xc1bdceee); // 4
    MD5_FF(a, b, c, d, _hash._w[4], md5::s11, 0xf57c0faf); // 5
    MD5_FF(d, a, b, c, _hash._w[5], md5::s12, 0x4787c62a); // 6
    MD5_FF(c, d, a, b, _hash._w[6], md5::s13, 0xa8304613); // 7
    MD5_FF(b, c, d, a, _hash._w[7], md5::s14, 0xfd469501); // 8
    MD5_FF(a, b, c, d, _hash._w[8], md5::s11, 0x698098d8); // 9
    MD5_FF(d, a, b, c, _hash._w[9], md5::s12, 0x8b44f7af); // 10
    MD5_FF(c, d, a, b, _hash._w[10], md5::s13, 0xffff5bb1); // 11
    MD5_FF(b, c, d, a, _hash._w[11], md5::s14, 0x895cd7be); // 12
    MD5_FF(a, b, c, d, _hash._w[12], md5::s11, 0x6b901122); // 13
    MD5_FF(d, a, b, c, _hash._w[13], md5::s12, 0xfd987193); // 14
    MD5_FF(c, d, a, b, _hash._w[14], md5::s13, 0xa679438e); // 15
    MD5_FF(b, c, d, a, _hash._w[15], md5::s14, 0x49b40821); // 16

    MD5_GG(a, b, c, d, _hash._w[1], md5::s21, 0xf61e2562); // 17
    MD5_GG(d, a, b, c, _hash._w[6], md5::s22, 0xc040b340); // 18
    MD5_GG(c, d, a, b, _hash._w[11], md5::s23, 0x265e5a51); // 19
    MD5_GG(b, c, d, a, _hash._w[0], md5::s24, 0xe9b6c7aa); // 20
    MD5_GG(a, b, c, d, _hash._w[5], md5::s21, 0xd62f105d); // 21
    MD5_GG(d, a, b, c, _hash._w[10], md5::s22, 0x02441453); // 22
    MD5_GG(c, d, a, b, _hash._w[15], md5::s23, 0xd8a1e681); // 23
    MD5_GG(b, c, d, a, _hash._w[4], md5::s24, 0xe7d3fbc8); // 24
    MD5_GG(a, b, c, d, _hash._w[9], md5::s21, 0x21e1cde6); // 25
    MD5_GG(d, a, b, c, _hash._w[14], md5::s22, 0xc33707d6); // 26
    MD5_GG(c, d, a, b, _hash._w[3], md5::s23, 0xf4d50d87); // 27
    MD5_GG(b, c, d, a, _hash._w[8], md5::s24, 0x455a14ed); // 28
    MD5_GG(a, b, c, d, _hash._w[13], md5::s21, 0xa9e3e905); // 29
    MD5_GG(d, a, b, c, _hash._w[2], md5::s22, 0xfcefa3f8); // 30
    MD5_GG(c, d, a, b, _hash._w[7], md5::s23, 0x676f02d9); // 31
    MD5_GG(b, c, d, a, _hash._w[12], md5::s24, 0x8d2a4c8a); // 32

    MD5_HH(a, b, c, d, _hash._w[5], md5::s31, 0xfffa3942); // 33
    MD5_HH(d, a, b, c, _hash._w[8], md5::s32, 0x8771f681); // 34
    MD5_HH(c, d, a, b, _hash._w[11], md5::s33, 0x6d9d6122); // 35
    MD5_HH(b, c, d, a, _hash._w[14], md5::s34, 0xfde5380c); // 36
    MD5_HH(a, b, c, d, _hash._w[1], md5::s31, 0xa4beea44); // 37
    MD5_HH(d, a, b, c, _hash._w[4], md5::s32, 0x4bdecfa9); // 38
    MD5_HH(c, d, a, b, _hash._w[7], md5::s33, 0xf6bb4b60); // 39
    MD5_HH(b, c, d, a, _hash._w[10], md5::s34, 0xbebfbc70); // 40
    MD5_HH(a, b, c, d, _hash._w[13], md5::s31, 0x289b7ec6); // 41
    MD5_HH(d, a, b, c, _hash._w[0], md5::s32, 0xeaa127fa); // 42
    MD5_HH(c, d, a, b, _hash._w[3], md5::s33, 0xd4ef3085); // 43
    MD5_HH(b, c, d, a, _hash._w[6], md5::s34, 0x04881d05); // 44
    MD5_HH(a, b, c, d, _hash._w[9], md5::s31, 0xd9d4d039); // 45
    MD5_HH(d, a, b, c, _hash._w[12], md5::s32, 0xe6db99e5); // 46
    MD5_HH(c, d, a, b, _hash._w[15], md5::s33, 0x1fa27cf8); // 47
    MD5_HH(b, c, d, a, _hash._w[2], md5::s34, 0xc4ac5665); // 48

    MD5_II(a, b, c, d, _hash._w[0], md5::s41, 0xf4292244); // 49
    MD5_II(d, a, b, c, _hash._w[7], md5::s42, 0x432aff97); // 50
    MD5_II(c, d, a, b, _hash._w[14], md5::s43, 0xab9423a7); // 51
    MD5_II(b, c, d, a, _hash._w[5], md5::s44, 0xfc93a039); // 52
    MD5_II(a, b, c, d, _hash._w[12], md5::s41, 0x655b59c3); // 53
    MD5_II(d, a, b, c, _hash._w[3], md5::s42, 0x8f0ccc92); // 54
    MD5_II(c, d, a, b, _hash._w[10], md5::s43, 0xffeff47d); // 55
    MD5_II(b, c, d, a, _hash._w[1], md5::s44, 0x85845dd1); // 56
    MD5_II(a, b, c, d, _hash._w[8], md5::s41, 0x6fa87e4f); // 57
    MD5_II(d, a, b, c, _hash._w[15], md5::s42, 0xfe2ce6e0); // 58
    MD5_II(c, d, a, b, _hash._w[6], md5::s43, 0xa3014314); // 59
    MD5_II(b, c, d, a, _hash._w[13], md5::s44, 0x4e0811a1); // 60
    MD5_II(a, b, c, d, _hash._w[4], md5::s41, 0xf7537e82); // 61
    MD5_II(d, a, b, c, _hash._w[11], md5::s42, 0xbd3af235); // 62
    MD5_II(c, d, a, b, _hash._w[2], md5::s43, 0x2ad7d2bb); // 63
    MD5_II(b, c, d, a, _hash._w[9], md5::s44, 0xeb86d391); // 64

    _hash._h[3] += a;
    _hash._h[2] += b;
    _hash._h[1] += c;
    _hash._h[0] += d;
}

void MD5::End()
{
    size_t len;
    uint32_t h;
    uint64_t bits = ((uint64_t)_blocks << 9) + ((uint64_t)_len << 3);
    _hash._m[_len++] = 0x80;
    if (_len == _blockBytes) {
        Transform();
        _len = 0;
    }
    len = _blockBytes - _len;
    if (len < 8) {
        memset(&_hash._m[_len], 0, len);
        Transform();
        memset(_hash._m, 0, _blockBytes - 8);
        _len = 0;
    } else
        memset(&_hash._m[_len], 0, len - 8);

    _hash._m[63] = (uint8_t)(bits >> 56);
    _hash._m[62] = (uint8_t)(bits >> 48);
    _hash._m[61] = (uint8_t)(bits >> 40);
    _hash._m[60] = (uint8_t)(bits >> 32);
    _hash._m[59] = (uint8_t)(bits >> 24);
    _hash._m[58] = (uint8_t)(bits >> 16);
    _hash._m[57] = (uint8_t)(bits >> 8);
    _hash._m[56] = (uint8_t)(bits);
    Transform();
    h = _hash._h[0];
    _hash._h[0] = _hash._h[3];
    _hash._h[3] = h;
    h = _hash._h[1];
    _hash._h[1] = _hash._h[2];
    _hash._h[2] = h;
}

uint8_t* MD5::PutOID(uint8_t* buf)
{
    uint8_t* q = buf;
    *q++ = asn::sequence;
    *q++ = 0x0c;
    *q++ = asn::oid;
    *q++ = 0x08;
    *q++ = OID_BYTE1(oid::iso, oid::isombr);
    *q++ = OID_HI(oid::isombrus);
    *q++ = OID_LO(oid::isombrus);
    *q++ = OID_HIHI(oid::isombrus_rsadsi);
    *q++ = OID_HI(oid::isombrus_rsadsi);
    *q++ = OID_LO(oid::isombrus_rsadsi);
    *q++ = oid::isombrus_rsadsi_dig;
    *q++ = oid::isombrus_rsadsi_digmd5;
    *q++ = 0;
    return q;
}

uint8_t* MD5::PutDigestInfo(uint8_t* buf)
{
    uint8_t* q = buf;
    *q++ = asn::sequence;
    *q++ = 0x20;
    q = PutOID(q);
    *q++ = asn::octetstring;
    *q++ = md5::hashbytes;
    memcpy(q, _hash._d, md5::hashbytes);
    q += md5::hashbytes;
    return q;
}
