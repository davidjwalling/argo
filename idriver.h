#pragma once

#include "iapi.h"

struct IDriver
{
};

EXPORT IDriver* TheDriverPtr();
EXPORT IDriver& TheDriver();
