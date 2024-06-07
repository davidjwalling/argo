#pragma once

#include "api.h"

#include <string>

class Path {
    std::string path;

public:
    EXPORT Path();
    EXPORT Path(const std::string&);
    EXPORT ~Path();
    EXPORT void Init();
    EXPORT void Reset();
    EXPORT void Clear();
    EXPORT std::string Parent() const;
    EXPORT std::string File() const;
    EXPORT bool EatFirst(std::string&);
    EXPORT Path& operator=(const Path&);
    EXPORT Path& operator=(const char*);
};
