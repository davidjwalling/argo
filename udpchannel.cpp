#include "udpchannel.h"

namespace udpchannel {
    enum {
        size = 65535
    };
}

UdpChannel::UdpChannel()
{
    //  A UDP channel does not require a connection. So it can immediately be
    //  placed into a need-first-bytes state. The size of the UDP channel read
    //  buffer must be at least big enough to hold the largest UDP packet.

    _state = channel::state::needfirstbytes;
    _read.reserve(udpchannel::size);
}

UdpChannel::~UdpChannel()
{
}

void UdpChannel::Read()
{
    size_t len = _read.capacity();
    _read.resize(len);
    _socket.Get(&_read[0], &len);
    _read.resize(len);
}

void UdpChannel::Write()
{
    size_t len = _write.size();
    if (len) {
        _socket.Put(&_write[0], &len);
        _write.erase(0, len);
    }
}

void UdpChannel::NeedFirstBytes()
{
    if (_write.size()) {
        Write();
    } else if (_read.size() < 1) {
        Read();
    } else {
        _read.clear();
    }
}
