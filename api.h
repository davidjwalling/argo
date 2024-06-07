#pragma once

#include "iapi.h"

#if defined(_WIN32)
#include <WinSock2.h> // SOCKET, DWORD, va_list, vsnprintf
#include <ws2ipdef.h> // sockaddr_in6
#include <WS2tcpip.h> // inet_pton
#endif
#include <cstdint> // uint16_t

enum {
    cond_base_socket = 300,
    cond_base_driver = 1000
};

#if defined(_WIN32)
#define WINSOCKVERSION 0x0202
#endif

#if defined(_WIN32)
using ssize_t = int;
#else
#if defined(__linux__)
using SOCKET = size_t;
#endif
