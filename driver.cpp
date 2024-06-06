#include "driver.h"

Driver theDriver;

Driver::Driver()
{
    Init();
}

Driver::~Driver()
{
    Reset();
}

void Driver::Init()
{
}

void Driver::Reset()
{
    Init();
}

IDriver& TheDriver()
{
    return theDriver;
}

IDriver* TheDriverPtr()
{
    return &theDriver;
}
