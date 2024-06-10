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

#if defined(_WIN32)
using ssize_t = int;
#else
#if defined(__linux__)
using SOCKET = size_t;
#else
using SOCKET = int;
#endif
#endif
