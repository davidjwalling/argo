#include "cipher.h"
#include "random.h"

void Cipher::SetIV()
{
    int n;
    uint8_t uc;
    uint8_t* p, * q;
    Random rnd;

    for (p = _iv, q = _ivt, n = cipher::ivsize; n; n--) {
        uc = (uint8_t)rnd.Rand();
        if (!uc)
            uc = 0x5a;
        *p++ = *q++ = uc;
    }
}

void Cipher::SetIV(uint8_t* iv)
{
    memcpy(_iv, iv, sizeof _iv);
    memcpy(_ivt, iv, sizeof _ivt);
}

void Cipher::GetKey(uint8_t* buf)
{
    memcpy(buf, _key, sizeof _key);
}
