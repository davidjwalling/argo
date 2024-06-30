#include "api.h"

#if !defined(_WIN32)
void GetSystemTime(SYSTEMTIME* system_time)
{
    time_t t;
    struct timeval tv = { 0 };
    struct tm* tm;
    gettimeofday(&tv, nullptr);
    t = tv.tv_sec;
    tm = gmtime(&t);
    system_time->wYear = tm->tm_year + 1900;
    system_time->wMonth = tm->tm_mon + 1;
    system_time->wDay = tm->tm_mday;
    system_time->wHour = tm->tm_hour;
    system_time->wMinute = tm->tm_min;
    system_time->wSecond = tm->tm_sec;
    system_time->wMilliseconds = tv.tv_usec / 1000;
}
#endif

void datetimeclock(uint8_t* out)
{
    time_t t = time(nullptr);
    clock_t c = clock();
    uint8_t* q = out;
    *q++ = ((t >> 24) & 255);
    *q++ = ((t >> 16) & 255);
    *q++ = ((t >> 8) & 255);
    *q++ = (t & 255);
    *q++ = ((c >> 24) & 255);
    *q++ = ((c >> 16) & 255);
    *q++ = ((c >> 8) & 255);
    *q++ = (c & 255);
}

void freeptr(uint8_t** ptr)
{
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = 0;
    }
}

void setptr(uint8_t** ptr, uint8_t* s, size_t len)
{
    if (!ptr)
        return;
    if ((s) && (s == *ptr))
        return;
    freeptr(ptr);
    if (!len)
        return;
    *ptr = (uint8_t*)calloc(len + 1, 1);
    if (!*ptr)
        return;
    if (s) {
        memcpy(*ptr, s, len);
        (*ptr)[len] = '\0';
    }
    else {
        memset(*ptr, 0, len + 1);
    }
}
