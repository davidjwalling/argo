#pragma once

#include "channel.h"

class UdpChannel : public Channel {
public:
    UdpChannel();
    ~UdpChannel();

    Socket& Sock() { return _socket; }
    void Sock(SOCKET sock) { _socket.Sock(sock); }
    void V6(bool v6) { _socket.V6(v6); }

    void Read() override;
    void Write() override;

    void NeedFirstBytes() override;
};
