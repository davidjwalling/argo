#pragma once

#include "idriver.h"

class Driver final
    : public IDriver
{
    void Init();
    void Reset();

public:
    Driver();
    ~Driver();
    int Result() override;
};
