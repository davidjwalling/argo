//  Copyright 2010 David J. Walling. All rights reserved.

#include "ascii.h"
#include "channel.h"
#include "logger.h"

#include <map>

namespace channel {
    namespace error {
        const char* missing_or_invalid_command = "command missing or invalid";
        const char* incomplete_destination_header = "incomplete destination header";
        const char* missing_source_header = "source header missing";
        const char* incomplete_source_header = "incomplete source header";
        const char* missing_command = "missing command";
        const char* invalid_command = "invalid command";
        const char* incomplete_parameters = "incomplete parameters";
        const char* invalid_termination = "invalid termination";
    }
    enum {
        size = 512
    };
}

namespace http {
    const char* day[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    const char* month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
}

const char* ok_0 = \
"HTTP/1.1 200 OK\r\nDate: %s\r\nServer: Argo/0.X\r\n" \
"Content-type: text/html\r\nContent-Length: %zd\r\n\r\n";

const char* ok_1 = \
"<!DOCTYPE html><html><head>" \
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">" \
"<style type=\"text/css\">" \
"body{background-color:#fafaf0;color:#2b2c30;margin:0;}" \
"div{padding:3px;font-family:Arial,sans-serif;font-size:1em;}" \
".titl{position:fixed;top:0;}" \
".copy{background-color:#2b2c30;color:#fafaf0;position:fixed;bottom:0;width:50%;}" \
".rlbl{right:0;text-align:right;}" \
"</style><title>Argo 0.X</title></head><body>" \
"<div class=\"titl\">Argo 0.X Experimental</div>" \
"<div class=\"copy\">&copy; 2010 David J. Walling</div>" \
"<div class=\"copy rlbl\">";

const char* ok_2 = \
"</div></body></html>";

Channel::Channel()
{
    //  Initialize members to default values.

    Init();
}

Channel::~Channel()
{
    Reset();
}

void Channel::Init()
{
    //  A channel is initialized here only when constructed or destructed. An
    //  existing channel can be cleared when it is simpley prepared for reuse.

    _next = nullptr;
    _prev = nullptr;
    _json = nullptr;

    Clear();

    _read.reserve(channel::size);
}

void Channel::Reset()
{
    //  To reset a channel, we need to reset its socket and then initialize
    //  members to default values.

    _socket.Reset();

    Init();
}

void Channel::Clear()
{
    //  Clearing the channel does not close or reset the socket. Here, we
    //  simply prepare to reuse the channel. Linked-list pointers _next and
    //  _prev, the JSON model, and the read buffer size are not affected.

    _expires = 0;
    _remain = 0;

    _tlsRecType = tls::rec::none;
    _tlsPlainTextVersion = tls::ver::none;
    _httpResponse = http::response::unspecified;
    _state = channel::state::ready;

    _method.clear();
    _resource.clear();
    _version.clear();

    _read.clear();
    _write.clear();
    _message.clear();
}

void Channel::Read()
{
    size_t len = _read.capacity();
    _read.resize(len);
    _socket.Read(&_read[0], &len);
    _read.resize(len);
}

void Channel::Write()
{
    size_t len = _write.size();
    if (len) {
        _socket.Write(&_write[0], &len);
        _write.erase(0, len);
    }
}

void Channel::NeedFirstBytes()
{
    //  Read until we have enough bytes to determine the protocol.

    if (_read.size() < 3) {
        if (_socket.Readable())
            Read();
    }
    
    //  If the first byte suggests a TLS handshake, we'll try that.

    else if (tls::rec::type::handshake == _read[0]) {
        _remain = 5;
        _state = channel::state::needplaintext;
    }
    
    //  An HTTP request is the next most likely. Check the first three bytes
    //  for a possible match with POST, PATCH, PUT, GET, HEAD, DELETE, OPTIONS,
    //  CONNECT, or TRACE.
    
    else if ((_P == _read[0] && ((_O == _read[1] && _S == _read[2]) || (_T == _read[2] && (_A == _read[1] || _U == _read[1]))))                     //  POST, PATCH, PUT
          || (_E == _read[1] && ((_G == _read[0] && _T == _read[2]) || (_H == _read[0] && _A == _read[2]) || (_D == _read[0] && _L == _read[2])))   //  GET, HEAD, DELETE
          || (_O == _read[0] && ((_P == _read[1] && _T == _read[2])))                                                                               //  OPTIONS
          || (_C == _read[0] && ((_O == _read[1] && _N == _read[2])))                                                                               //  CONNECT
          || (_T == _read[0] && ((_R == _read[1] && _A == _read[2])))) {                                                                            //  TRACE
        _state = channel::state::needhttprequest;
    }

    //  If the protocol is not recognized, close the TCP connection and put the
    //  channel back into ready state.

    else {
        Close();
    }
}

void Channel::NeedPlainText()
{
    if (_read.size() < 5) {
        Read();
        if (_read.size() < 5)
            return;
    }

}

void Channel::HavePlainText()
{
}

void Channel::SendAlert(const char level, const char description)
{
    //  To-Do: Add encrypted alerts

    _write.resize(7);
    char* p = &_write[0];
    *p++ = tls::rec::alert;
    *p++ = 3;
    *p++ = 0;
    *p++ = 0;
    *p++ = 2;
    *p++ = level;
    *p++ = description;
}


void Channel::NeedHttpRequest()
{
    //  Write as much pending output as we can. If not all pending output data
    //  can be written immediately, service any other channel and then retry.

    if (_write.size()) {
        Write();
        if (_write.size())
            return;
    }

    //  With all pending output written, try to read data. If no data can be
    //  read immediately, service any other channel and then retry.

    if (_read.empty()) {
        Read();
        if (_read.empty())
            return;
    }

    //  Append data to the message until LF. Close the connection if the message
    //  exceeds the maximum allowable length.

    size_t n = _read.find(_lf);
    if (n == std::string::npos) {
        _message += _read;
        _read.clear();
        if (_message.size() > http::max::len::request) {
            Close();
        }
    } else {
        _message += _read.substr(0, n + 1);
        _read.erase(0, n + 1);
        if (_message.size() > http::max::len::request) {
            Close();
        } else {
            _state = channel::state::havehttprequest;
        }
    }
}

bool Channel::ParseHttpRequest()
{
    //  Clear method, resource and version.

    _method.clear();
    _resource.clear();
    _version.clear();

    //  The method immediately starts and will end with one SP.

    char* p = &_message[0];
    for (; _A <= *p && _Z >= *p; _method += *p++);
    if (_method.empty() || _sp != *p)
        return false;

    //  The resource immediately follows and ends with one SP.

    for (++p; *p && _sp != *p && _ht != *p && _cr != *p && _lf != *p; _resource += *p++);
    if (_resource.empty() || _sp != *p)
        return false;

    //  The version immediately follows and ends with CR LF.

    for (++p; *p && _sp != *p && _ht != *p && _cr != *p && _lf != *p; _version += *p++);
    if (_version.empty() || _cr != *p)
        return false;
    return _lf == *(p + 1) && _nul == *(p + 2);
}

void Channel::HaveHttpRequest()
{
    ParseHttpRequest();

    std::time_t t = std::time(0);
    std::tm m{ 0 };
    memcpy(&m, gmtime(&t), sizeof(m));
    char date[30] = { 0 };
    sprintf(date, "%s, %u %s %u %02u:%02u:%02u GMT",
        http::day[m.tm_wday], m.tm_mday, http::month[m.tm_mon], m.tm_year + 1900,
        m.tm_hour, m.tm_min, m.tm_sec);

    char ip[INET6_ADDRSTRLEN] = { 0 };
    _socket.RemoteAddress(ip);

    char body[768] = { 0 };
    sprintf(body, "%s%s%s",
        ok_1, ip, ok_2);
    char hdr[256] = { 0 };
    sprintf(hdr, ok_0, date, strlen(body));

    _write = hdr;
    _write += body;

    Expires(std::time(0) + channel::expires);
    _state = channel::state::needhttprequest;
}

void Channel::Close()
{
    _socket.Reset();
    Clear();
}

void Channel::Service()
{
    if (_expires && time(0) > _expires) {
        _state = channel::state::close;
    }
    switch (_state) {
    case channel::state::needfirstbytes:
        NeedFirstBytes();
        break;
    case channel::state::needhttprequest:
        NeedHttpRequest();
        break;
    case channel::state::havehttprequest:
        HaveHttpRequest();
        break;
    case channel::state::close:
        Close();
        break;
    default:
        break;
    }
}
