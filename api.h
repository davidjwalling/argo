#pragma once

#include "iapi.h"

#if defined(_WIN32)
#include <WinSock2.h> // SOCKET, DWORD, va_list, vsnprintf
#include <ws2ipdef.h> // sockaddr_in6
#include <WS2tcpip.h> // inet_pton
#else
#if defined(__linux__) || defined (__APPLE__)
#include <fcntl.h> // open
#include <netinet/in.h> // sockaddr_in
#include <signal.h> // signal
#endif
#include <arpa/inet.h> // inet_pton
#include <cstdarg> // va_start, va_end
#include <cstring> // memset
#include <unistd.h> // fork
#endif
#include <cstdint> // uint16_t
#include <cstdio> // printf
#include <time.h> // time, gmtime

enum {
    cond_base_socket = 300,
    cond_base_driver = 1000
};

#if defined(_WIN32)
#define WINSOCKVERSION 0x0202
#else
#if defined(__linux__) || defined(__APPLE__)
#define closesocket(x) ::close(x)
#endif
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define WSAENOBUFS ENOBUFS
#define WSAENOTCONN ENOTCONN
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAGetLastError() errno
#endif
#define HILO32(x) ((x << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | (x >> 24))
#define HILO64(x) ((x << 56) | ((x & 0xFF00) << 40) | ((x & 0xFF0000) << 24) | ((x & 0xFF000000) << 8) | ((x >> 8) & 0xFF000000) | ((x >> 24) & 0xFF0000) | ((x >> 40) & 0xFF00) | (x >> 56))

#if defined(_WIN32)
using ssize_t = int;
#else
#if defined(__linux__)
using SOCKET = size_t;
#else
using SOCKET = int;
#endif
#endif
