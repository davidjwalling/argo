#pragma once

#include "api.h"

class Socket {
    SOCKET _socket;

    uint16_t _localPort;
    uint16_t _remotePort;

    struct sockaddr_in _remoteAddr4;
    struct sockaddr_in6 _remoteAddr6;

    bool _v6;

public:
    Socket();
    ~Socket();
    void Init();
    void Reset();

    SOCKET Sock() { return _socket; }
    uint16_t LocalPort() { return _localPort; }
    uint16_t RemotePort() { return _remotePort; }
    bool V6() { return _v6; }

    void RemoteAddress(char* addr);
    void RemoteAddress(struct sockaddr_in& addr) { addr = _remoteAddr4; }
    void RemoteAddress(struct sockaddr_in6& addr) { addr = _remoteAddr6; }

    void Port(uint16_t port) { _localPort = port; }
    void V6(bool v6) { _v6 = v6; }
    void Sock(SOCKET socket) { _socket = socket; }
    void SetAddressAndPortFrom(Socket& socket);

    bool Readable();
    bool Writable();
    bool Unblock(SOCKET sock);
    bool Stream(bool v6);
    bool Datagram(bool v6);
    bool Bind();
    bool Listen();
    SOCKET Accept();
    bool Read(char* buf, size_t* ret);
    bool Write(char* buf, size_t* ret);
    bool Get(char* buf, size_t* ret);
    bool Put(char* buf, size_t* ret);
    bool Shutdown();
    bool Close();
};
