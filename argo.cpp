#include "idriver.h"

int __cdecl main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    IDriver& driver = TheDriver();
    return driver.Result();
}