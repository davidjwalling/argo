//  Copyright 2010 David J. Walling. All rights reserved.

#include "json.h"

Json::Json()
{
    //  Initialize members to default values.

    Init();
}

Json::~Json()
{
    //  Release resources and initialize members.

    Reset();
}

void Json::Init()
{
    // Initialize members to default values.

    _parent = nullptr;
    _next = nullptr;
    _prev = nullptr;
    _first = nullptr;
    _last = nullptr;

    Clear();
}

void Json::Reset()
{
    // Release resources and remove ourself from the chain.

    if (_next)
        _next->_prev = _prev;
    if (_prev)
        _prev->_next = _next;

    Init();
}

void Json::Clear()
{
    //  A Json can be a parent of other Json in a hierarchy. Reset and
    //  delete all of them. Then initialize as needed for reuse.

    Json* next = nullptr;
    for (Json* n = _first; n; n = next) {
        next = n->_next;
        n->Reset();
        delete n;
    }

     _isArray = false;

     _name.clear();
    _stringValue.clear();
}

Json* Json::Descend(Json* j)
{
    Json* k = new Json;
    if (k) {
        k->_parent = j;
        if (j->_last) {
            j->_last->_next = k;
            k->_prev = j->_last;
        }
        j->_last = k;
        if (!j->_first) {
            j->_first = k;
        }
    }
    return k;
}

Json* Json::Extend(Json* j)
{
    Json* k = new Json;
    if (k) {
        j->_next = k;
        if (j->_parent) {
            k->_parent = j->_parent;
            k->_parent->_last = k;
        }
        k->_prev = j;
    }
    return k;
}

bool Json::Parse(const std::string& val)
{
    // Clear the existing parse, which frees resources allocated to build the
    // hierarchical doubly-linked list. Exit if no new JSON string to parse.

    Clear();
    if (val.empty())
        return false;

    // Setup tracking values.

    bool quoted = false;
    bool escaped = false;

    const char* n0 = nullptr; // start of name
    const char* n1 = nullptr; // end of name
    const char* v0 = nullptr; // start of value
    const char* v1 = nullptr; // end of value

    json::state state = json::state::need_context;

    // The "current" context moves as we parse into hierarchical structures
    // such as JSON objects and arrays. The initial context is 'this'.

    Json* j = this;

    // If there is a UTF-8 Byte Order Mark (BOM), it must be valid.

    const char* p = &val[0];
    if ((uint8_t)*p == 0xef) {
        if ((val.size() < 3) || ((uint8_t) * (p + 1) != 0xbb) || ((uint8_t) * (p + 2) != 0xbf)) {
            return false;
        }
        p += 3;
    }

    // Other BOMs are not yet supported:
    // 0xff 0xfe           = UTF-16 Little-Endian
    // 0xfe 0xff           = UTF-16 Big-Endian
    // 0xff 0xfe 0x00 0x00 = UTF-32 Little-Endian
    // 0x00 0x00 0xfe 0xff = UTF-32 Big-Endian
    // 0x2b 0x2f 0x76      = UTF-7 type 1
    // 0x2b 0x2f 0x76 0x38 = UTF-7 type 2

    else if (((uint8_t)*p == 0xff) || ((uint8_t)*p == 0xfe) || ((uint8_t)*p == 0x00) || ((uint8_t)*p == 0x2b)) {
        return false;
    }

    // Parse the remaining JSON. Remining UTF-8 will be quoted.

    for (; *p; p++) {
        switch (state) {

        // At the start of any parse, we need to find our context, which will
        // begin with a '{', '[' or "\"" after any optional spacing. If the
        // context is an object ('{') or an array ('['), we increase one level,
        // creating a child Json and parsing it. If a quote is found, this will
        // begin a name unless the parent is an array, in which case we start a
        // value.

        case json::state::need_context:
            if ((' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p))
                break;
            if ('{' == *p) {
                j = Descend(j);
                if (!j)
                    return false;
                state = json::state::need_name_begin;
            } else if ('[' == *p) {
                j->_isArray = true;
                j = Descend(j);
                if (!j)
                    return false;
                state = json::state::need_value_begin;
            } else if ('\"' == *p) {
                quoted = true;
                if (j->_parent && j->_parent->_isArray) {
                    v0 = p + 1;
                    state = json::state::need_value_string_end;
                } else {
                    n0 = p + 1;
                    state = json::state::need_name_end;
                }
            } else if (j->_parent && j->_parent->_isArray) {
                v0 = p;
                state = json::state::need_value_end;
            } else {
                return false;
            }
            break;

        // If we need the start of the member name, look for a double-quote. If
        // found, the name starts in the next position and we will search for
        // the end of the name. It can be that the object or array are empty.

        case json::state::need_name_begin:
            if ((' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p))
                break;
            if ('\"' == *p) {
                quoted = true;
                n0 = p + 1;
                state = json::state::need_name_end;
            } else if (('}' == *p) || (']' == *p)) {
                if (!j->_parent)
                    return false;
                if (('}' == *p) && (j->_parent->_isArray))
                    return false;
                if ((']' == *p) && (!j->_parent->_isArray))
                    return false;
                j = j->_parent;
                if (!j)
                    return false;
                state = json::state::need_member_end;
            } else {
                return false;
            }
            break;

        // If we need the end of the member name, let the backslash toggle an
        // escaped condition. If we find a double-quote and are not escaped,
        // save the name and start looking for the delimiter (':').

        case json::state::need_name_end:
            if ('\"' == *p) {
                if (!escaped) {
                    quoted = false;
                    n1 = p;
                    j->_name = val.substr(n0 - &val[0], n1 - n0);
                    state = json::state::need_name_delim;
                }
            } else if ('\\' == *p) {
                escaped = !escaped;
            }
            break;

        // If we need the delimiter after the name, check for the colon or for
        // leading spacing.

        case json::state::need_name_delim:
            if ((' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p))
                break;
            if (':' == *p) {
                state = json::state::need_value_begin;
            } else {
                return false;
            }
            break;

        // A value may start a new object, array or string.

        case json::state::need_value_begin:
            if ((' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p))
                break;
            if ('{' == *p) {
                j = Descend(j);
                if (!j)
                    return false;
                state = json::state::need_name_begin;
            } else if ('[' == *p) {
                j->_isArray = true;
                j = Descend(j);
                if (!j)
                    return false;
                state = json::state::need_value_begin;
            } else if (('}' == *p) || (']' == *p)) {
                if (!j->_parent)
                    return false;
                if (('}' == *p) && (j->_parent->_isArray))
                    return false;
                if ((']' == *p) && (!j->_parent->_isArray))
                    return false;
                j = j->_parent;
                if (!j)
                    return false;
                state = json::state::need_member_end;
            } else if ('\"' == *p) {
                quoted = true;
                v0 = p + 1;
                state = json::state::need_value_string_end;
            } else {
                v0 = p;
                state = json::state::need_value_end;
            }
            break;

        // A unquoted value (boolean, number) ends with the first comma or
        // spacing. Bump pos back 1 position, however, so that when we go to
        // need_member_end, we will find the comma and start the next member.

        case json::state::need_value_end:
            if ((',' == *p) || (' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p)) {
                v1 = p--;
                j->_stringValue = val.substr(v0 - &val[0], v1 - v0);
                state = json::state::need_member_end;
            }
            break;

        // A quoted value (string) ends with an unescaped quote.

        case json::state::need_value_string_end:
            if ('\"' == *p) {
                if (!escaped) {
                    quoted = false;
                    v1 = p;
                    j->_stringValue = val.substr(v0 - &val[0], v1 - v0);
                    state = json::state::need_member_end;
                }
            } else if ('\\' == *p) {
                escaped = !escaped;
            }
            break;

        // A JSON member ends with a delimiting comma, where there is a series
        // of JSON objects, events, values or the end of an array or object,
        // where we return to the next higher level.

        case json::state::need_member_end:
            if ((' ' == *p) || ('\n' == *p) || ('\t' == *p) || ('\r' == *p))
                break;
            if (',' == *p) {
                j = Extend(j);
                if (!j)
                    return false;
                state = json::state::need_context;
            } else if ((']' == *p) || ('}' == *p)) {
                j = j->_parent;
                if (!j)
                    return false;
                state = json::state::need_member_end;
            } else if (*p)
                return false;
            break;
        default:
            break;
        }
    }
    return(!quoted && !escaped);
}

bool Json::Get(Path& path, std::string& val)
{
    Json* j = this;
    Json* k = nullptr;
    std::string node;
    while (path.EatFirst(node)) {
        for (k = j->_first; k; k = k->_next) {
            if (node == k->_name)
                break;
        }
        if (!k)
            break;
        j = k;
    }
    if (!k)
        return false;
    val = k->_stringValue;
    return true;
}

bool Json::Get(Path& path, const Json** json)
{
    Json* j = this;
    Json* k = nullptr;
    std::string node;
    while (path.EatFirst(node)) {
        for (k = j->_first; k; k = k->_next) {
            if (node == k->_name)
                break;
        }
        if (!k)
            break;
        j = k;
    }
    if (!k)
        return false;
    *json = k;
    return true;
}
