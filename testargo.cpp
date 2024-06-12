#include "iapi.h"
#include "md5.h"
#include "sha.h"

#include <iostream>

using namespace std;

typedef struct _digestTest {
    const char* label;
    int count;
    uint8_t* vector;
    uint8_t* digest;
    int len;
    int iter;
} DIGESTTEST;

namespace testargo {
    const char* all = "all";
    const char* dump = "dump";
    const char* test = "test";
}

const char Bin2AscHex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const char Bin2AscPrt[] = {
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

//  MD5 test vectors 1-7: RFC 1321

uint8_t* const md5_vector_1 = (uint8_t*)"";
uint8_t* const md5_vector_2 = (uint8_t*)"a";
uint8_t* const md5_vector_3 = (uint8_t*)"abc";
uint8_t* const md5_vector_4 = (uint8_t*)"message digest";
uint8_t* const md5_vector_5 = (uint8_t*)"abcdefghijklmnopqrstuvwxyz";
uint8_t* const md5_vector_6 = (uint8_t*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
uint8_t* const md5_vector_7 = (uint8_t*)"12345678901234567890123456789012345678901234567890123456789012345678901234567890";

uint8_t* const md5_digest_1 = (uint8_t*)"\xD4\x1D\x8C\xD9\x8F\x00\xB2\x04\xE9\x80\x09\x98\xEC\xF8\x42\x7E";
uint8_t* const md5_digest_2 = (uint8_t*)"\x0C\xC1\x75\xB9\xC0\xF1\xB6\xA8\x31\xC3\x99\xE2\x69\x77\x26\x61";
uint8_t* const md5_digest_3 = (uint8_t*)"\x90\x01\x50\x98\x3C\xD2\x4F\xB0\xD6\x96\x3F\x7D\x28\xE1\x7F\x72";
uint8_t* const md5_digest_4 = (uint8_t*)"\xF9\x6B\x69\x7D\x7C\xB7\x93\x8D\x52\x5A\x2F\x31\xAA\xF1\x61\xD0";
uint8_t* const md5_digest_5 = (uint8_t*)"\xC3\xFC\xD3\xD7\x61\x92\xE4\x00\x7D\xFB\x49\x6C\xCA\x67\xE1\x3B";
uint8_t* const md5_digest_6 = (uint8_t*)"\xD1\x74\xAB\x98\xD2\x77\xD9\xF5\xA5\x61\x1C\x2C\x9F\x41\x9D\x9F";
uint8_t* const md5_digest_7 = (uint8_t*)"\x57\xED\xF4\xA2\x2B\xE3\xC9\x55\xAC\x49\xDA\x2E\x21\x07\xB6\x7A";

DIGESTTEST md5_tests[] = {
    { "MD5", 7, md5_vector_1, md5_digest_1, 0, 1 },
    { "MD5", 7, md5_vector_2, md5_digest_2, 1, 1 },
    { "MD5", 7, md5_vector_3, md5_digest_3, 3, 1 },
    { "MD5", 7, md5_vector_4, md5_digest_4, 14, 1 },
    { "MD5", 7, md5_vector_5, md5_digest_5, 26, 1 },
    { "MD5", 7, md5_vector_6, md5_digest_6, 62, 1 },
    { "MD5", 7, md5_vector_7, md5_digest_7, 80, 1 }
};

// SHA1 test vectors 1-3: FIPS pub 180-2 appendix A examples 1-3
// SHA1 test vectors 4-6: NIST pub SHAVS appendix A.1 examples 2, 4 and appendix A.2 example 2

uint8_t* const sha1_vector_1 = (uint8_t*)"abc";
uint8_t* const sha1_vector_2 = (uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
uint8_t* const sha1_vector_3 = (uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
uint8_t* const sha1_vector_4 = (uint8_t*)"\x5e";
uint8_t* const sha1_vector_5 = (uint8_t*)"\x9a\x7d\xfd\xf1\xec\xea\xd0\x6e\xd6\x46\xaa\x55\xfe\x75\x71\x46";
uint8_t* const sha1_vector_6 = (uint8_t*)"\xf7\x8f\x92\x14\x1b\xcd\x17\x0a\xe8\x9b\x4f\xba\x15\xa1\xd5\x9f\x3f\xd8\x4d\x22\x3c\x92\x51\xbd\xac\xbb\xae\x61\xd0\x5e\xd1\x15"
"\xa0\x6a\x7c\xe1\x17\xb7\xbe\xea\xd2\x44\x21\xde\xd9\xc3\x25\x92\xbd\x57\xed\xea\xe3\x9c\x39\xfa\x1f\xe8\x94\x6a\x84\xd0\xcf\x1f"
"\x7b\xee\xad\x17\x13\xe2\xe0\x95\x98\x97\x34\x7f\x67\xc8\x0b\x04\x00\xc2\x09\x81\x5d\x6b\x10\xa6\x83\x83\x6f\xd5\x56\x2a\x56\xca"
"\xb1\xa2\x8e\x81\xb6\x57\x66\x54\x63\x1c\xf1\x65\x66\xb8\x6e\x3b\x33\xa1\x08\xb0\x53\x07\xc0\x0a\xff\x14\xa7\x68\xed\x73\x50\x60"
"\x6a\x0f\x85\xe6\xa9\x1d\x39\x6f\x5b\x5c\xbe\x57\x7f\x9b\x38\x80\x7c\x7d\x52\x3d\x6d\x79\x2f\x6e\xbc\x24\xa4\xec\xf2\xb3\xa4\x27"
"\xcd\xbb\xfb";

uint8_t* const sha1_digest_1 = (uint8_t*)"\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2\x6C\x9C\xD0\xD8\x9D";
uint8_t* const sha1_digest_2 = (uint8_t*)"\x84\x98\x3E\x44\x1C\x3B\xD2\x6E\xBA\xAE\x4A\xA1\xF9\x51\x29\xE5\xE5\x46\x70\xF1";
uint8_t* const sha1_digest_3 = (uint8_t*)"\x34\xAA\x97\x3C\xD4\xC4\xDA\xA4\xF6\x1E\xEB\x2B\xDB\xAD\x27\x31\x65\x34\x01\x6F";
uint8_t* const sha1_digest_4 = (uint8_t*)"\x5e\x6f\x80\xa3\x4a\x97\x98\xca\xfc\x6a\x5d\xb9\x6c\xc5\x7b\xa4\xc4\xdb\x59\xc2";
uint8_t* const sha1_digest_5 = (uint8_t*)"\x82\xab\xff\x66\x05\xdb\xe1\xc1\x7d\xef\x12\xa3\x94\xfa\x22\xa8\x2b\x54\x4a\x35";
uint8_t* const sha1_digest_6 = (uint8_t*)"\xcb\x00\x82\xc8\xf1\x97\xd2\x60\x99\x1b\xa6\xa4\x60\xe7\x6e\x20\x2b\xad\x27\xb3";

DIGESTTEST sha1_tests[] = {
    { "SHA1", 6, sha1_vector_1, sha1_digest_1, 3, 1 },
    { "SHA1", 6, sha1_vector_2, sha1_digest_2, 56, 1 },
    { "SHA1", 6, sha1_vector_3, sha1_digest_3, 100, 10000 },
    { "SHA1", 6, sha1_vector_4, sha1_digest_4, 1, 1 },
    { "SHA1", 6, sha1_vector_5, sha1_digest_5, 16, 1 },
    { "SHA1", 6, sha1_vector_6, sha1_digest_6, 163, 1 }
};

// SHA256 test vectors 1-3: FIPS pub 180-2 appendix B examples 1-3
// SHA256 test vectors 4-6: NIST pub SHAVS appendix C.1 examples 2, 4 and appendix C.2 example 2

uint8_t* const sha256_vector_1 = (uint8_t*)"abc";
uint8_t* const sha256_vector_2 = (uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
uint8_t* const sha256_vector_3 = (uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
uint8_t* const sha256_vector_4 = (uint8_t*)"\x19";
uint8_t* const sha256_vector_5 = (uint8_t*)"\xe3\xd7\x25\x70\xdc\xdd\x78\x7c\xe3\x88\x7a\xb2\xcd\x68\x46\x52";
uint8_t* const sha256_vector_6 = (uint8_t*)"\x83\x26\x75\x4e\x22\x77\x37\x2f\x4f\xc1\x2b\x20\x52\x7a\xfe\xf0\x4d\x8a\x05\x69\x71\xb1\x1a\xd5\x71\x23\xa7\xc1\x37\x76\x00\x00"
"\xd7\xbe\xf6\xf3\xc1\xf7\xa9\x08\x3a\xa3\x9d\x81\x0d\xb3\x10\x77\x7d\xab\x8b\x1e\x7f\x02\xb8\x4a\x26\xc7\x73\x32\x5f\x8b\x23\x74"
"\xde\x7a\x4b\x5a\x58\xcb\x5c\x5c\xf3\x5b\xce\xe6\xfb\x94\x6e\x5b\xd6\x94\xfa\x59\x3a\x8b\xeb\x3f\x9d\x65\x92\xec\xed\xaa\x66\xca"
"\x82\xa2\x9d\x0c\x51\xbc\xf9\x33\x62\x30\xe5\xd7\x84\xe4\xc0\xa4\x3f\x8d\x79\xa3\x0a\x16\x5c\xba\xbe\x45\x2b\x77\x4b\x9c\x71\x09"
"\xa9\x7d\x13\x8f\x12\x92\x28\x96\x6f\x6c\x0a\xdc\x10\x6a\xad\x5a\x9f\xdd\x30\x82\x57\x69\xb2\xc6\x71\xaf\x67\x59\xdf\x28\xeb\x39"
"\x3d\x54\xd6";

uint8_t* const sha256_digest_1 = (uint8_t*)"\xba\x78\x16\xbf\x8f\x01\xcf\xea\x41\x41\x40\xde\x5d\xae\x22\x23\xb0\x03\x61\xa3\x96\x17\x7a\x9c\xb4\x10\xff\x61\xf2\x00\x15\xad";
uint8_t* const sha256_digest_2 = (uint8_t*)"\x24\x8d\x6a\x61\xd2\x06\x38\xb8\xe5\xc0\x26\x93\x0c\x3e\x60\x39\xa3\x3c\xe4\x59\x64\xff\x21\x67\xf6\xec\xed\xd4\x19\xdb\x06\xc1";
uint8_t* const sha256_digest_3 = (uint8_t*)"\xcd\xc7\x6e\x5c\x99\x14\xfb\x92\x81\xa1\xc7\xe2\x84\xd7\x3e\x67\xf1\x80\x9a\x48\xa4\x97\x20\x0e\x04\x6d\x39\xcc\xc7\x11\x2c\xd0";
uint8_t* const sha256_digest_4 = (uint8_t*)"\x68\xaa\x2e\x2e\xe5\xdf\xf9\x6e\x33\x55\xe6\xc7\xee\x37\x3e\x3d\x6a\x4e\x17\xf7\x5f\x95\x18\xd8\x43\x70\x9c\x0c\x9b\xc3\xe3\xd4";
uint8_t* const sha256_digest_5 = (uint8_t*)"\x17\x5e\xe6\x9b\x02\xba\x9b\x58\xe2\xb0\xa5\xfd\x13\x81\x9c\xea\x57\x3f\x39\x40\xa9\x4f\x82\x51\x28\xcf\x42\x09\xbe\xab\xb4\xe8";
uint8_t* const sha256_digest_6 = (uint8_t*)"\x97\xdb\xca\x7d\xf4\x6d\x62\xc8\xa4\x22\xc9\x41\xdd\x7e\x83\x5b\x8a\xd3\x36\x17\x63\xf7\xe9\xb2\xd9\x5f\x4f\x0d\xa6\xe1\xcc\xbc";

DIGESTTEST sha256_tests[] = {
    { "SHA256", 6, sha256_vector_1, sha256_digest_1, 3, 1 },
    { "SHA256", 6, sha256_vector_2, sha256_digest_2, 56, 1 },
    { "SHA256", 6, sha256_vector_3, sha256_digest_3, 100, 10000 },
    { "SHA256", 6, sha256_vector_4, sha256_digest_4, 1, 1 },
    { "SHA256", 6, sha256_vector_5, sha256_digest_5, 16, 1 },
    { "SHA256", 6, sha256_vector_6, sha256_digest_6, 163, 1 }
};

// SHA224 test vectors 1-3: FIPS pub 180-2 change notice 1 examples 1-3
// SHA224 test vectors 4-6: NIST pub SHAVS appendix B.1 examples 2, 4 and appendix B.2 example 2

uint8_t* const sha224_vector_1 = (uint8_t*)"abc";
uint8_t* const sha224_vector_2 = (uint8_t*)"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
uint8_t* const sha224_vector_3 = (uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
uint8_t* const sha224_vector_4 = (uint8_t*)"\x07";
uint8_t* const sha224_vector_5 = (uint8_t*)"\x18\x80\x40\x05\xdd\x4f\xbd\x15\x56\x29\x9d\x6f\x9d\x93\xdf\x62";
uint8_t* const sha224_vector_6 = (uint8_t*)"\x55\xb2\x10\x07\x9c\x61\xb5\x3a\xdd\x52\x06\x22\xd1\xac\x97\xd5\xcd\xbe\x8c\xb3\x3a\xa0\xae\x34\x45\x17\xbe\xe4\xd7\xba\x09\xab"
"\xc8\x53\x3c\x52\x50\x88\x7a\x43\xbe\xbb\xac\x90\x6c\x2e\x18\x37\xf2\x6b\x36\xa5\x9a\xe3\xbe\x78\x14\xd5\x06\x89\x6b\x71\x8b\x2a"
"\x38\x3e\xcd\xac\x16\xb9\x61\x25\x55\x3f\x41\x6f\xf3\x2c\x66\x74\xc7\x45\x99\xa9\x00\x53\x86\xd9\xce\x11\x12\x24\x5f\x48\xee\x47"
"\x0d\x39\x6c\x1e\xd6\x3b\x92\x67\x0c\xa5\x6e\xc8\x4d\xee\xa8\x14\xb6\x13\x5e\xca\x54\x39\x2b\xde\xdb\x94\x89\xbc\x9b\x87\x5a\x8b"
"\xaf\x0d\xc1\xae\x78\x57\x36\x91\x4a\xb7\xda\xa2\x64\xbc\x07\x9d\x26\x9f\x2c\x0d\x7e\xdd\xd8\x10\xa4\x26\x14\x5a\x07\x76\xf6\x7c"
"\x87\x82\x73";

uint8_t* const sha224_digest_1 = (uint8_t*)"\x23\x09\x7d\x22\x34\x05\xd8\x22\x86\x42\xa4\x77\xbd\xa2\x55\xb3\x2a\xad\xbc\xe4\xbd\xa0\xb3\xf7\xe3\x6c\x9d\xa7";
uint8_t* const sha224_digest_2 = (uint8_t*)"\x75\x38\x8b\x16\x51\x27\x76\xcc\x5d\xba\x5d\xa1\xfd\x89\x01\x50\xb0\xc6\x45\x5c\xb4\xf5\x8b\x19\x52\x52\x25\x25";
uint8_t* const sha224_digest_3 = (uint8_t*)"\x20\x79\x46\x55\x98\x0c\x91\xd8\xbb\xb4\xc1\xea\x97\x61\x8a\x4b\xf0\x3f\x42\x58\x19\x48\xb2\xee\x4e\xe7\xad\x67";
uint8_t* const sha224_digest_4 = (uint8_t*)"\x00\xec\xd5\xf1\x38\x42\x2b\x8a\xd7\x4c\x97\x99\xfd\x82\x6c\x53\x1b\xad\x2f\xca\xbc\x74\x50\xbe\xe2\xaa\x8c\x2a";
uint8_t* const sha224_digest_5 = (uint8_t*)"\xdf\x90\xd7\x8a\xa7\x88\x21\xc9\x9b\x40\xba\x4c\x96\x69\x21\xac\xcd\x8f\xfb\x1e\x98\xac\x38\x8e\x56\x19\x1d\xb1";
uint8_t* const sha224_digest_6 = (uint8_t*)"\x0b\x31\x89\x4e\xc8\x93\x7a\xd9\xb9\x1b\xdf\xbc\xba\x29\x4d\x9a\xde\xfa\xa1\x8e\x09\x30\x5e\x9f\x20\xd5\xc3\xa4";

DIGESTTEST sha224_tests[] = {
    { "SHA224", 6, sha224_vector_1, sha224_digest_1, 3, 1 },
    { "SHA224", 6, sha224_vector_2, sha224_digest_2, 56, 1 },
    { "SHA224", 6, sha224_vector_3, sha224_digest_3, 100, 10000 },
    { "SHA224", 6, sha224_vector_4, sha224_digest_4, 1, 1 },
    { "SHA224", 6, sha224_vector_5, sha224_digest_5, 16, 1 },
    { "SHA224", 6, sha224_vector_6, sha224_digest_6, 163, 1 }
};

// SHA512 test vectors 1-3: FIPS pub 180-2 appendix C examples 1-3
// SHA512 test vectors 4-6: NIST pub SHAVS appendix E.1 examples 2, 4 and appendix E.2 example 2

uint8_t* const sha512_vector_1 = (uint8_t*)"abc";
uint8_t* const sha512_vector_2 = (uint8_t*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
uint8_t* const sha512_vector_3 = (uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
uint8_t* const sha512_vector_4 = (uint8_t*)"\xd0";
uint8_t* const sha512_vector_5 = (uint8_t*)"\x8d\x4e\x3c\x0e\x38\x89\x19\x14\x91\x81\x6e\x9d\x98\xbf\xf0\xa0";
uint8_t* const sha512_vector_6 = (uint8_t*)"\xa5\x5f\x20\xc4\x11\xaa\xd1\x32\x80\x7a\x50\x2d\x65\x82\x4e\x31\xa2\x30\x54\x32\xaa\x3d\x06\xd3\xe2\x82\xa8\xd8\x4e\x0d\xe1\xde"
"\x69\x74\xbf\x49\x54\x69\xfc\x7f\x33\x8f\x80\x54\xd5\x8c\x26\xc4\x93\x60\xc3\xe8\x7a\xf5\x65\x23\xac\xf6\xd8\x9d\x03\xe5\x6f\xf2"
"\xf8\x68\x00\x2b\xc3\xe4\x31\xed\xc4\x4d\xf2\xf0\x22\x3d\x4b\xb3\xb2\x43\x58\x6e\x1a\x7d\x92\x49\x36\x69\x4f\xcb\xba\xf8\x8d\x95"
"\x19\xe4\xeb\x50\xa6\x44\xf8\xe4\xf9\x5e\xb0\xea\x95\xbc\x44\x65\xc8\x82\x1a\xac\xd2\xfe\x15\xab\x49\x81\x16\x4b\xbb\x6d\xc3\x2f"
"\x96\x90\x87\xa1\x45\xb0\xd9\xcc\x9c\x67\xc2\x2b\x76\x32\x99\x41\x9c\xc4\x12\x8b\xe9\xa0\x77\xb3\xac\xe6\x34\x06\x4e\x6d\x99\x28"
"\x35\x13\xdc\x06\xe7\x51\x5d\x0d\x73\x13\x2e\x9a\x0d\xc6\xd3\xb1\xf8\xb2\x46\xf1\xa9\x8a\x3f\xc7\x29\x41\xb1\xe3\xbb\x20\x98\xe8"
"\xbf\x16\xf2\x68\xd6\x4f\x0b\x0f\x47\x07\xfe\x1e\xa1\xa1\x79\x1b\xa2\xf3\xc0\xc7\x58\xe5\xf5\x51\x86\x3a\x96\xc9\x49\xad\x47\xd7"
"\xfb\x40\xd2";

uint8_t* const sha512_digest_1 = (uint8_t*)"\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b\x55\xd3\x9a"
"\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c\x23\xa3\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a\x9a\xc9\x4f\xa5\x4c\xa4\x9f";
uint8_t* const sha512_digest_2 = (uint8_t*)"\x8e\x95\x9b\x75\xda\xe3\x13\xda\x8c\xf4\xf7\x28\x14\xfc\x14\x3f\x8f\x77\x79\xc6\xeb\x9f\x7f\xa1\x72\x99\xae\xad\xb6\x88\x90\x18"
"\x50\x1d\x28\x9e\x49\x00\xf7\xe4\x33\x1b\x99\xde\xc4\xb5\x43\x3a\xc7\xd3\x29\xee\xb6\xdd\x26\x54\x5e\x96\xe5\x5b\x87\x4b\xe9\x09";
uint8_t* const sha512_digest_3 = (uint8_t*)"\xe7\x18\x48\x3d\x0c\xe7\x69\x64\x4e\x2e\x42\xc7\xbc\x15\xb4\x63\x8e\x1f\x98\xb1\x3b\x20\x44\x28\x56\x32\xa8\x03\xaf\xa9\x73\xeb"
"\xde\x0f\xf2\x44\x87\x7e\xa6\x0a\x4c\xb0\x43\x2c\xe5\x77\xc3\x1b\xeb\x00\x9c\x5c\x2c\x49\xaa\x2e\x4e\xad\xb2\x17\xad\x8c\xc0\x9b";
uint8_t* const sha512_digest_4 = (uint8_t*)"\x99\x92\x20\x29\x38\xe8\x82\xe7\x3e\x20\xf6\xb6\x9e\x68\xa0\xa7\x14\x90\x90\x42\x3d\x93\xc8\x1b\xab\x3f\x21\x67\x8d\x4a\xce\xee"
"\xe5\x0e\x4e\x8c\xaf\xad\xa4\xc8\x5a\x54\xea\x83\x06\x82\x6c\x4a\xd6\xe7\x4c\xec\xe9\x63\x1b\xfa\x8a\x54\x9b\x4a\xb3\xfb\xba\x15";
uint8_t* const sha512_digest_5 = (uint8_t*)"\xcb\x0b\x67\xa4\xb8\x71\x2c\xd7\x3c\x9a\xab\xc0\xb1\x99\xe9\x26\x9b\x20\x84\x4a\xfb\x75\xac\xbd\xd1\xc1\x53\xc9\x82\x89\x24\xc3"
"\xdd\xed\xaa\xfe\x66\x9c\x5f\xdd\x0b\xc6\x6f\x63\x0f\x67\x73\x98\x82\x13\xeb\x1b\x16\xf5\x17\xad\x0d\xe4\xb2\xf0\xc9\x5c\x90\xf8";
uint8_t* const sha512_digest_6 = (uint8_t*)"\xc6\x65\xbe\xfb\x36\xda\x18\x9d\x78\x82\x2d\x10\x52\x8c\xbf\x3b\x12\xb3\xee\xf7\x26\x03\x99\x09\xc1\xa1\x6a\x27\x0d\x48\x71\x93"
"\x77\x96\x6b\x95\x7a\x87\x8e\x72\x05\x84\x77\x9a\x62\x82\x5c\x18\xda\x26\x41\x5e\x49\xa7\x17\x6a\x89\x4e\x75\x10\xfd\x14\x51\xf5";

DIGESTTEST sha512_tests[] = {
    { "SHA512", 6, sha512_vector_1, sha512_digest_1, 3, 1 },
    { "SHA512", 6, sha512_vector_2, sha512_digest_2, 112, 1 },
    { "SHA512", 6, sha512_vector_3, sha512_digest_3, 100, 10000 },
    { "SHA512", 6, sha512_vector_4, sha512_digest_4, 1, 1 },
    { "SHA512", 6, sha512_vector_5, sha512_digest_5, 16, 1 },
    { "SHA512", 6, sha512_vector_6, sha512_digest_6, 227, 1 }
};

// SHA384 test vectors 1-3: FIPS pub 180-2 appendix D examples 1-3
// SHA384 test vectors 4-6: NIST pub SHAVS appendix D.1 examples 2, 4 and appendix D.2 example 2

uint8_t* const sha384_vector_1 = (uint8_t*)"abc";
uint8_t* const sha384_vector_2 = (uint8_t*)"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
uint8_t* const sha384_vector_3 = (uint8_t*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
uint8_t* const sha384_vector_4 = (uint8_t*)"\xb9";
uint8_t* const sha384_vector_5 = (uint8_t*)"\xa4\x1c\x49\x77\x79\xc0\x37\x5f\xf1\x0a\x7f\x4e\x08\x59\x17\x39";
uint8_t* const sha384_vector_6 = (uint8_t*)"\x39\x96\x69\xe2\x8f\x6b\x9c\x6d\xbc\xbb\x69\x12\xec\x10\xff\xcf\x74\x79\x03\x49\xb7\xdc\x8f\xbe\x4a\x8e\x7b\x3b\x56\x21\xdb\x0f"
"\x3e\x7d\xc8\x7f\x82\x32\x64\xbb\xe4\x0d\x18\x11\xc9\xea\x20\x61\xe1\xc8\x4a\xd1\x0a\x23\xfa\xc1\x72\x7e\x72\x02\xfc\x3f\x50\x42"
"\xe6\xbf\x58\xcb\xa8\xa2\x74\x6e\x1f\x64\xf9\xb9\xea\x35\x2c\x71\x15\x07\x05\x3c\xf4\xe5\x33\x9d\x52\x86\x5f\x25\xcc\x22\xb5\xe8"
"\x77\x84\xa1\x2f\xc9\x61\xd6\x6c\xb6\xe8\x95\x73\x19\x9a\x2c\xe6\x56\x5c\xbd\xf1\x3d\xca\x40\x38\x32\xcf\xcb\x0e\x8b\x72\x11\xe8"
"\x3a\xf3\x2a\x11\xac\x17\x92\x9f\xf1\xc0\x73\xa5\x1c\xc0\x27\xaa\xed\xef\xf8\x5a\xad\x7c\x2b\x7c\x5a\x80\x3e\x24\x04\xd9\x6d\x2a"
"\x77\x35\x7b\xda\x1a\x6d\xae\xed\x17\x15\x1c\xb9\xbc\x51\x25\xa4\x22\xe9\x41\xde\x0c\xa0\xfc\x50\x11\xc2\x3e\xcf\xfe\xfd\xd0\x96"
"\x76\x71\x1c\xf3\xdb\x0a\x34\x40\x72\x0e\x16\x15\xc1\xf2\x2f\xbc\x3c\x72\x1d\xe5\x21\xe1\xb9\x9b\xa1\xbd\x55\x77\x40\x86\x42\x14"
"\x7e\xd0\x96";

uint8_t* const sha384_digest_1 = (uint8_t*)"\xcb\x00\x75\x3f\x45\xa3\x5e\x8b\xb5\xa0\x3d\x69\x9a\xc6\x50\x07\x27\x2c\x32\xab\x0e\xde\xd1\x63\x1a\x8b\x60\x5a\x43\xff\x5b\xed"
"\x80\x86\x07\x2b\xa1\xe7\xcc\x23\x58\xba\xec\xa1\x34\xc8\x25\xa7";
uint8_t* const sha384_digest_2 = (uint8_t*)"\x09\x33\x0c\x33\xf7\x11\x47\xe8\x3d\x19\x2f\xc7\x82\xcd\x1b\x47\x53\x11\x1b\x17\x3b\x3b\x05\xd2\x2f\xa0\x80\x86\xe3\xb0\xf7\x12"
"\xfc\xc7\xc7\x1a\x55\x7e\x2d\xb9\x66\xc3\xe9\xfa\x91\x74\x60\x39";
uint8_t* const sha384_digest_3 = (uint8_t*)"\x9d\x0e\x18\x09\x71\x64\x74\xcb\x08\x6e\x83\x4e\x31\x0a\x4a\x1c\xed\x14\x9e\x9c\x00\xf2\x48\x52\x79\x72\xce\xc5\x70\x4c\x2a\x5b"
"\x07\xb8\xb3\xdc\x38\xec\xc4\xeb\xae\x97\xdd\xd8\x7f\x3d\x89\x85";
uint8_t* const sha384_digest_4 = (uint8_t*)"\xbc\x80\x89\xa1\x90\x07\xc0\xb1\x41\x95\xf4\xec\xc7\x40\x94\xfe\xc6\x4f\x01\xf9\x09\x29\x28\x2c\x2f\xb3\x92\x88\x15\x78\x20\x8a"
"\xd4\x66\x82\x8b\x1c\x6c\x28\x3d\x27\x22\xcf\x0a\xd1\xab\x69\x38";
uint8_t* const sha384_digest_5 = (uint8_t*)"\xc9\xa6\x84\x43\xa0\x05\x81\x22\x56\xb8\xec\x76\xb0\x05\x16\xf0\xdb\xb7\x4f\xab\x26\xd6\x65\x91\x3f\x19\x4b\x6f\xfb\x0e\x91\xea"
"\x99\x67\x56\x6b\x58\x10\x9c\xbc\x67\x5c\xc2\x08\xe4\xc8\x23\xf7";
uint8_t* const sha384_digest_6 = (uint8_t*)"\x4f\x44\x0d\xb1\xe6\xed\xd2\x89\x9f\xa3\x35\xf0\x95\x15\xaa\x02\x5e\xe1\x77\xa7\x9f\x4b\x4a\xaf\x38\xe4\x2b\x5c\x4d\xe6\x60\xf5"
"\xde\x8f\xb2\xa5\xb2\xfb\xd2\xa3\xcb\xff\xd2\x0c\xff\x12\x88\xc0";

DIGESTTEST sha384_tests[] = {
    { "SHA384", 6, sha384_vector_1, sha384_digest_1, 3, 1 },
    { "SHA384", 6, sha384_vector_2, sha384_digest_2, 112, 1 },
    { "SHA384", 6, sha384_vector_3, sha384_digest_3, 100, 10000 },
    { "SHA384", 6, sha384_vector_4, sha384_digest_4, 1, 1 },
    { "SHA384", 6, sha384_vector_5, sha384_digest_5, 16, 1 },
    { "SHA384", 6, sha384_vector_6, sha384_digest_6, 227, 1 }
};

bool _all = false;
bool _dump = false;

void queryRun(const char* prompt, void(*fn)())
{
    char ch[3] = { 0 };
    if (!_all) {
        printf("Run %s Tests? (y/N) ", prompt);
        fflush(stdin);
        fgets(ch, sizeof(ch), stdin);
        if (('y' == ch[0]) || ('Y' == ch[0]))
            fn();
    } else
        fn();
}

void dumpMem(uint8_t* p, size_t len)
{
    uint16_t adr = 0;
    uint16_t col = 0;
    uint8_t byte;
    char line[73];
    memset(line, ' ', sizeof(line));
    line[54] = '|';
    line[71] = '|';
    line[72] = '\0';
    for (; len; adr += 16) {
        line[0] = Bin2AscHex[(adr >> 12) & 15];
        line[1] = Bin2AscHex[(adr >> 8) & 15];
        line[2] = Bin2AscHex[(adr >> 4) & 15];
        line[3] = Bin2AscHex[(adr) & 15];
        for (col = 0; col < 16; col++) {
            if (len) {
                len--;
                byte = *p++;
                line[col + 55] = Bin2AscPrt[byte];
                line[col * 3 + 6] = Bin2AscHex[(byte >> 4) & 15];
                line[col * 3 + 7] = Bin2AscHex[(byte) & 15];
            } else {
                line[col + 55] = ' ';
                line[col * 3 + 6] = ' ';
                line[col * 3 + 7] = ' ';
            }
        }
        printf("%s\n", line);
    }
}

void testDigest(Digest& digest, uint8_t* hash, DIGESTTEST* tests)
{
    printf("Testing %s\n", tests[0].label);
    if (!_dump)
        printf("\n");
    for (int x = 0; x < tests[0].count; x++) {
        digest.Begin();
        for (int i = tests[x].iter; i; i--)
            digest.Update(tests[x].vector, tests[x].len);
        digest.End();
        digest.GetDigest(hash);
        if (_dump) {
            printf("\n");
            printf("Vector (iterated %i %s):\n", tests[x].iter, tests[x].iter > 1 ? "times" : "time");
            dumpMem(tests[x].vector, tests[x].len);
            printf("Digest:\n");
            dumpMem(hash, digest._hashBytes);
            printf("\n");
        }
        printf("%-5s %s %i of %i\n", memcmp(hash, tests[x].digest, digest._hashBytes) ? "FAIL" : "OK", tests[x].label, x + 1, tests[x].count);
    }
    printf("\n");
}

void testMD5()
{
    uint8_t hash[md5::hashbytes] = { 0 };
    MD5 digest;
    testDigest(digest, hash, md5_tests);
}

void testSHA1()
{
    uint8_t hash[sha1::hashbytes] = { 0 };
    SHA1 sha;
    testDigest(sha, hash, sha1_tests);
}

void testSHA256()
{
    uint8_t hash[sha256::hashbytes] = { 0 };
    SHA256 sha;
    testDigest(sha, hash, sha256_tests);
}

void testSHA224()
{
    uint8_t hash[sha224::hashbytes] = { 0 };
    SHA224 sha;
    testDigest(sha, hash, sha224_tests);
}

void testSHA512()
{
    uint8_t hash[sha512::hashbytes] = { 0 };
    SHA512 sha;
    testDigest(sha, hash, sha512_tests);
}

void testSHA384()
{
    uint8_t hash[sha384::hashbytes] = { 0 };
    SHA384 sha;
    testDigest(sha, hash, sha384_tests);
}

void testDigestAlgs()
{
    queryRun(md5_tests[0].label, testMD5);
    queryRun(sha1_tests[0].label, testSHA1);
    queryRun(sha256_tests[0].label, testSHA256);
    queryRun(sha224_tests[0].label, testSHA224);
    queryRun(sha512_tests[0].label, testSHA512);
    queryRun(sha384_tests[0].label, testSHA384);
}

int runTests()
{
    printf("\n");
    printf("sizeof(short)     %2zd bytes\n", sizeof(short));
    printf("sizeof(int)       %2zd bytes\n", sizeof(int));
    printf("sizeof(long)      %2zd bytes\n", sizeof(long));
    printf("sizeof(long long) %2zd bytes\n", sizeof(long long));
    printf("\n");

    queryRun("Message Digest", testDigestAlgs);

    printf("Tests Completed\n");
    return 0;
}

int usage()
{
    printf("Usage: testargo [all] [dump]\n");
    return 0;
}

int __cdecl main(int argc, char* argv[])
{
    printf("Argo Test Program [0.X]\n");
    printf("Copyright 2010 David J. Walling. MIT License.\n");
    for (int n = 1; n < argc; n++) {
        if (!strcmp(argv[n], testargo::all))
            _all = true;
        else if (!strcmp(argv[n], testargo::dump))
            _dump = true;
        else
            return usage();
    }
    return runTests();
}