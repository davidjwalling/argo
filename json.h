#pragma once

#include "api.h"
#include "path.h"

#include <string>

namespace json {
    enum class state : unsigned int {
        need_context = 0,
        need_name_begin,
        need_name_end,
        need_name_delim,

        need_value_begin,
        need_value_end,
        need_value_string_end,
        need_value_array_end,
        need_value_object_end,
        need_member_end
    };
}

class Json {
    void Init();
    void Reset();
    void Clear();

public:
    Json* _parent;
    Json* _next;
    Json* _prev;
    Json* _first;
    Json* _last;

    bool _isArray;
    std::string _name;
    std::string _stringValue;

    Json();
    ~Json();
    Json* Descend(Json* j);
    Json* Extend(Json* j);
    bool Parse(const std::string& val);
    bool Get(Path& path, std::string& val);
    bool Get(Path& path, const Json** json);
};
