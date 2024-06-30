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