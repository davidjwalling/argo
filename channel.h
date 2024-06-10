#pragma once

#include "json.h"
#include "socket.h"

#include <string>

namespace hctl {
    namespace max {
        enum len {
            message = 65536
        };
    }
}

namespace http {
    enum class method : uint8_t {
        none = 0,
        connect,
        delete_,
        get,
        head,
        options,
        patch,
        post,
        put,
        trace
    };
    enum class response : uint16_t {
        unspecified = 0,
        ok = 200,
        badrequest = 400,
        badversion = 505
    };
    namespace max {
        enum len {
            request = 1024
        };
    }
}

namespace tls {
    namespace rec {
        enum type : char {
            none = 0,
            change = 20,
            alert = 21,
            handshake = 22,
            data = 23
        };
    }
    enum ver : uint16_t {
        none = 0,
        ssl30 = 0x0300,
        tls10 = 0x0301,
        tls11 = 0x0302,
        tls12 = 0x0303,
        tls13 = 0x0304
    };
}

namespace channel {
    enum class state {
        ready = 0,
        needfirstbytes,
        needplaintext,
        haveplaintext,
        needhttprequest,
        havehttprequest,
        close
    };
    enum {
        expires = 7200 // 7200 = two hours for debugging
    };
}

class Channel {
    Channel* _next;
    Channel* _prev;

    time_t _expires;
    size_t _remain;

    uint8_t _tlsRecType;
    uint16_t _tlsPlainTextVersion;

    http::response _httpResponse;

    std::string _method;
    std::string _resource;
    std::string _version;

    void NeedPlainText();
    void HavePlainText();

    void SendAlert(const char level, const char description);

    void NeedHttpRequest();
    void HaveHttpRequest();
    bool ParseHttpRequest();

    virtual void NeedFirstBytes();

protected:
    channel::state _state;

    std::string _read;
    std::string _write;
    std::string _message;

    Json* _json;
    Socket _socket;

public:
    Channel();
    virtual ~Channel();

    void Init();
    void Reset();
    void Clear();
    void Close();

    Channel* Next() { return _next; }
    Channel* Prev() { return _prev; }
    channel::state State() { return _state; }

    void Next(Channel* next) { _next = next; }
    void Prev(Channel* prev) { _prev = prev; }
    void State(channel::state state) { _state = state; }
    void Expires(time_t expires) { _expires = expires; }

    void SetJson(Json& json) { _json = &json; }
    void Sock(SOCKET sock) { _socket.Sock(sock); }
    void SetAddressAndPortFrom(Socket& socket) { _socket.SetAddressAndPortFrom(socket); }

    virtual void Read();
    virtual void Write();
    virtual void Service();
};
