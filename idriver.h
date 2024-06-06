#pragma once

#include "iapi.h"

struct IDriver
{
	virtual int Result() = 0;
};

EXPORT IDriver* TheDriverPtr();
EXPORT IDriver& TheDriver();
