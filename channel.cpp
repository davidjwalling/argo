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

namespace hctl {
    namespace command {
        const char* disco = "disco";
        const char* get = "get";
        const char* set = "set";
    }
}

namespace http {
    const char* day[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
    const char* month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
}

const char* ok_0 = \
"HTTP/1.1 200 OK\r\nDate: %s\r\nServer: Ava/0.X\r\n" \
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
"</style><title>Ava 0.X</title></head><body>" \
"<div class=\"titl\">Ava 0.X Experimental</div>" \
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

    _target.clear();
    _source.clear();
    _command.clear();
    _params.clear();

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
    
    //  HCTL might start with forwarding headers. Otherwise, an HCTL command
    //  will be lower case or a reply starting wiht the 'at' sign.
    
    else if ((_lbrace == _read[0]) || (_a <= _read[0] && _z >= _read[0]) || _at == _read[0]) {
        _state = channel::state::needhctlmessage;
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

    //  The version immediatley follows and ends with CR LF.

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

void Channel::NeedHctlRequest()
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
    if (std::string::npos == n) {
        _message += _read;
        _read.clear();
        if (_message.size() > hctl::max::len::message) {
            Close();
        }
    } else {
        _message += _read.substr(0, n + 1);
        _read.erase(0, n + 1);
        if (_message.size() > hctl::max::len::message) {
            Close();
        } else {
            _state = channel::state::havehctlmessage;
        }
    }
}

bool Channel::ParseHctlRequest()
{
    //  Clear the target and source headers, command and parameters.

    _target.clear();
    _source.clear();
    _command.clear();
    _params.clear();

    //  Spin over leading SP, HT. Done if EOL.

    char* p = &_message[0];

    //  Forwarding headers are optional but exist if the first non-
    //  white space is a left brace. If present, complete target and
    //  source headers must be found.

    if (_lbrace == *p) {

        //  NUL, right brace, CR or LF ends the target.

        for (++p; *p && _rbrace != *p && _cr != *p && _lf != *p; p++)
            _target += *p;
        if (_rbrace != *p)
            return false;

        //  Spacing may follow the target but must end with left brace.

        for (++p; _sp == *p || _ht == *p; p++);
        if (_lbrace != *p)
            return false;

        //  NUL, right brace, CR or LF ends the source.

        for (++p; *p && _rbrace != *p && _cr != *p && _lf != *p; p++)
            _source += *p;
        if (_rbrace != *p)
            return false;

        //  Spacing may follow the source but it cannot end the message.

        for (++p; _sp == *p || _ht == *p; p++);
        if (_nul == *p || _cr == *p || _lf == *p)
            return false;
    }

    //  The command must be lower case alpha and end with spacing or EOL.

    for (; *p && _a <= *p && _z >= *p; p++)
        _command += *p;
    if (_command.empty())
        return false;
    if (*p && _sp != *p && _ht != *p && _cr != *p && _lf != *p)
        return false;

    //  Spin over spacing to either EOL or start of parameters. If there are
    //  parameters, they must be a valid JSON object.

    for (++p; _sp == *p || _ht == *p; p++);
    if (_nul == *p || _cr == *p || _lf == *p)
        return true;
    if (_lbracket != *p)
        return false;

    //  If there are parameters, it must be a valid JSON object.

    _params += *p;
    int level = 1;
    bool quote = 0;
    for (++p; *p && _cr != *p && _lf != *p; _params += *p++) {
        if (_dquote == *p)
            quote = !quote;
        else if (_lbracket == *p)
            level++;
        else if (_rbracket == *p) {
            if (!quote) {
                if (!level)
                    return false;
                level--;
            }
        }
    }
    return level == 0;
}

void Channel::HaveHctlRequest()
{
    bool ret = ParseHctlRequest();
    _message.clear();
    Expires(std::time(0) + channel::expires);
    _state = channel::state::needhctlmessage;
    if (!ret) {
        _write += "@error {\"error\":\"invalid message\"}\n";
        return;
    }
    _write = "@" + _command;
    if (hctl::command::disco == _command) {
        if (_params.empty()) {
            _write += " {}\n";
            return;
        }
        Json params;
        if (!params.Parse(_params)) {
            _write += " {\"error\":\"invalid parameters\"}\n";
            return;
        }
        Path path("params");
        const Json* json = nullptr;
        if (!params.Get(path, &json)) {
            _write += " {\"error\":\"missing params\"}\n";
            return;
        }
        if (!json || !json->_isArray) {
            _write += " {\"error\":\"params not an array\"}\n";
            return;
        }
        _write += " {";
        for (const Json* k = json->_first; k; k = k->_next) {
            Path param("/configuration/device/" + k->_stringValue);
            std::string val;
            if (_json->Get(param, val)) {
                _write += "\"" + k->_stringValue + "\":\"" + val + "\",";
            }
        }
        if (_write.back() == ',')
            _write.pop_back();
        _write += "}";
    }
    _write += "\n";
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
    case channel::state::needhctlmessage:
        NeedHctlRequest();
        break;
    case channel::state::havehctlmessage:
        HaveHctlRequest();
        break;
    case channel::state::close:
        Close();
        break;
    default:
        break;
    }
}
