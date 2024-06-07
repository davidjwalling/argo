#include "idriver.h"

#include <iostream>
#include <string>

using namespace std;

namespace argo {
    namespace command {
        constexpr const char* exit = "exit";
    }
    constexpr const char* config = "argo.cfg";
    constexpr const char* copyright = "Copyright 2010 David J. Walling. MIT License.";
    constexpr const char* description = "Argo Secure Protocol Service";
    constexpr const char* display = "Argo";
    constexpr const char* name = "argo";
    constexpr const char* registry = "SOFTWARE\\Proserio\\Argo\\";
    constexpr const char* spec = "argo.exe service";
    constexpr const char* title = "Argo 0.X Experimental";
    constexpr const char* usage = "usage: argo [install [name]|uninstall [name]]";
}

//  Program control enters at main after any global instances in the library are
//  constructed. The program prefers stdcall on 32-bit but main must be __cdecl.

int __cdecl main(int argc, char* argv[])
{
    //  The singleton Driver is constructed in the Ava library. Obtain a
    //  reference to it and set its members to application-layer values.

    IDriver& driver = TheDriver();

    driver.Config(argo::config);
    driver.Copyright(argo::copyright);
    driver.Description(argo::description);
    driver.Display(argo::display);
    driver.Name(argo::name);
    driver.Registry(argo::registry);
    driver.Spec(argo::spec);
    driver.Title(argo::title);
    driver.Usage(argo::usage);

    //  Library initialization only returns true if the program completes
    //  configuration and will run in the foreground. If the program will
    //  run as a service or daemon, the background process will have been
    //  forked and this parent process may exit. Start also returns false
    //  if an error occurred durring initialization.

    if (driver.Start(argc, argv)) {
        string line;
        while (getline(cin, line) && line != argo::command::exit);
        driver.Stop();
    }

    //  The program will exit returning any result code maintained in the
    //  library. But, global instance destructors are called on exit from
    //  main.

    return driver.Result();
}
