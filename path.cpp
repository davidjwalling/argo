#include "path.h"

Path::Path()
{
    Init();
}

Path::Path(const std::string& val)
{
    Init();

    path = val;
}

Path::~Path()
{
    Reset();
}

void Path::Init()
{
}

void Path::Reset()
{
    Clear();
    Init();
}

void Path::Clear()
{
    path.clear();
}

std::string Path::Parent() const
{
    size_t found = path.find_last_of("/\\");
    if (std::string::npos != found) {
        return path.substr(0, found);
    }
    return "";
}

std::string Path::File() const
{
    size_t found = path.find_last_of("/\\");
    if (std::string::npos != found) {
        return path.substr(found + 1);
    }
    return path;
}

bool Path::EatFirst(std::string& first)
{
    first.clear();
    if (!path.empty() && (path[0] == '/' || path[0] == '\\')) {
        path = path.substr(1);
    }
    size_t found = path.find_first_of("/\\");
    if (std::string::npos != found) {
        first = path.substr(0, found);
        path = path.substr(found + 1);
        return true;
    }
    if (!path.empty()) {
        first = path;
        path.clear();
        return true;
    }
    return false;
}

Path& Path::operator=(const Path& other)
{
    if (this != &other) {
        path = other.path;
    }
    return *this;
}

Path& Path::operator=(const char* other)
{
    path = other;
    return *this;
}
