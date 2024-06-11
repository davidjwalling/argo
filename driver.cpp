#include "channel.h"
#include "driver.h"
#include "logger.h"

#include <chrono>
#include <fstream>
#include <sstream>

using namespace std;

namespace cond {
    namespace driver{
        enum {
            servicestart = cond_base_driver,
            title,
            copyright,
            usage,

            //  Service installation

            installing,
            access,
            opensc,
            lock,
            size,
            module,
            file,
            command,
            exists,
            display,
            create,
            key,
            value,
            closekey,
            close,
            unlock,
            closesc,
            installed,

            //  Service uninstalation

            uninstalling,
            missing,
            name,
            open,
            remove,
            uninstalled,

            //  Service dispatch

            console,
            dispatch,

            // Linux/macOS Daemon Startup

            pipe1,
            alarm1,
            intr,
            fork,
            setsid,
            emptyset,
            addset,
            procmask,
            term,
            pipe2,
            alarm2,
            hup,
            null,
            stdin_,
            stdout_,
            stderr_,

            //  Windows startup

            wsastartup,

            //  Configuration

            regopenkeyex,
            reggetvalue,
            regsetting,
            regclosekey,
            envsetting,
            cfgsetting,
            argsetting,
            modelloaded,
            complete,

            //  Shutdown

            servicestop,
            stopping,
            finalize
        };
        namespace message {
            constexpr const char* servicestart = "Service started";

            //  Service installation

            constexpr const char* installing = "Installing service";
            constexpr const char* access = "Administrative privilege is required";
            constexpr const char* opensc = "Unable to open service control manager";
            constexpr const char* lock = "LockServiceDatabase error";
            constexpr const char* size = "Registry name too long";
            constexpr const char* module = "GetModuleHandle error";
            constexpr const char* file = "GetModuleFileName error";
            constexpr const char* command = "Service command line too long";
            constexpr const char* exists = "Service already exists";
            constexpr const char* display = "Service display name in use";
            constexpr const char* create = "CreateService error";
            constexpr const char* key = "RegCreateKeyEx error";
            constexpr const char* value = "RegSetValueEx error";
            constexpr const char* closekey = "RegCloseKey error";
            constexpr const char* close = "CloseServiceHandle error";
            constexpr const char* unlock = "UnlockServiceDatabase error";
            constexpr const char* closesc = "CloseServiceHandle error";
            constexpr const char* installed = "Service installed";

            //  Service uninstallation

            constexpr const char* uninstalling = "Uninstalling service";
            constexpr const char* missing = "Service not found";
            constexpr const char* name = "Invalid service name";
            constexpr const char* open = "OpenService error";
            constexpr const char* remove = "DeleteService error";
            constexpr const char* uninstalled = "Service uninstalled";

            //  Service dispatch

            constexpr const char* console = "Service cannot start from console";
            constexpr const char* dispatch = "StartServiceCtrlDispatcher error";

            //  Linux/macOS Daemon dispatch

            constexpr const char* pipe1 = "sigaction(SIGPIPE)(1) error";
            constexpr const char* alarm1 = "sigaction(SIGALRM)(1) error";
            constexpr const char* intr = "sigaction(INT) error";
            constexpr const char* fork = "form error";
            constexpr const char* setsid = "setsid error";
            constexpr const char* emptyset = "sigemptyset error";
            constexpr const char* addset = "sigaddset error";
            constexpr const char* procmask = "sigprocmask error";
            constexpr const char* term = "sigaction(SIGTERM) error";
            constexpr const char* pipe2 = "sigaction(SIGPIPE)(2) error";
            constexpr const char* alarm2 = "sigaction(SIGALRM)(2) error";
            constexpr const char* hup = "sigaction(SIGHUP) error";
            constexpr const char* null = "open(/dev/null) error";
            constexpr const char* stdin_ = "dup2(STDIN) error";
            constexpr const char* stdout_ = "dup2(STDOUT) error";
            constexpr const char* stderr_ = "dup2(STDERR) error";

            //  Configuration

            constexpr const char* regopenkeyex = "RegOpenKeyEx error";
            constexpr const char* reggetvalue = "RegGetValue error";
            constexpr const char* regsetting = "I%04d Setting %s to (%s) from registry";
            constexpr const char* regclosekey = "RegCloseKey error";
            constexpr const char* envsetting = "I%04d Setting %s to (%s) from environment";
            constexpr const char* cfgsetting = "I%04d Setting %s to (%s) from config file";
            constexpr const char* argsetting = "I%04d Setting &s to (%s) from program arguments";
            constexpr const char* modelloaded = "Model loaded";
            constexpr const char* complete = "Configuration completed";

            //  Shutdown

            constexpr const char* servicestop = "Service stopping";
            constexpr const char* stopping = "Exiting main loop";
            constexpr const char* finalize = "Finalization completed";
        }
    }
}

namespace driver {
    enum {
        port = 4197,
        stopwait = 100
    };
    namespace arg {
        constexpr const char* configfile = "-f";
        constexpr const char* descriptor = "-d";
        constexpr const char* loglevel = "-l";
        constexpr const char* logpath = "-g";
        constexpr const char* logprefix = "-e";
        constexpr const char* logsuffix = "-s";
        constexpr const char* model = "-m";
        constexpr const char* port = "-p";
    }
    namespace env {
        constexpr const char* descriptor = "DESCRIPTOR";
        constexpr const char* loglevel = "LOGLEVEL";
        constexpr const char* logpath = "LOGPATH";
        constexpr const char* logprefix = "LOGPREFIX";
        constexpr const char* logsuffix = "LOGSUFFIX";
    }
    namespace setting {
        enum {
            configfile = 1,
            descriptor,
            loglevel,
            logpath,
            logprefix,
            logsuffix,
            model,
            port
        };
        namespace name {
            constexpr const char* configfile = "config-file";
            constexpr const char* descriptor = "descriptor";
            constexpr const char* loglevel = "log-level";
            constexpr const char* logpath = "log-path";
            constexpr const char* logprefix = "log-prefix";
            constexpr const char* logsuffix = "log-suffix";
            constexpr const char* model = "model";
            constexpr const char* port = "port";
        }
    }
    constexpr const char* install = "install";
    constexpr const char* logsuffix = "log";
    constexpr const char* service = "service";
    constexpr const char* uninstall = "uninstall";
}

typedef struct setting {
    const char* str;
    uint32_t id;
} SETTING;

constexpr const SETTING app_settings[] = {
    {driver::setting::name::descriptor, driver::setting::descriptor},
    {driver::setting::name::loglevel, driver::setting::loglevel},
    {driver::setting::name::logpath, driver::setting::logpath},
    {driver::setting::name::logprefix, driver::setting::logprefix},
    {driver::setting::name::logsuffix, driver::setting::logsuffix},
    {0, 0}
};

//  There is only ever one one instance of the Driver. It is constructed when
//  the library is loaded, before control reaches main. It is destructed when
//  the library is unloaded. Two public APIs return a reference or pointer to
//  the Driver, TheDriver and TheDriverPtr.

Driver theDriver;

Driver::Driver()
{
    //  Initialize members to default values.

    Init();
}

Driver::~Driver()
{
    //  Release resources and initialize members.

    Reset();
}

void Driver::Init()
{
    //  Initialize members to default values both when the class is created
    //  and when it is destroyed so that no runtime member value remains in
    //  memory when the program ends.

    _stopping = false;
    _stopped = false;
    _daemon = false;
    _winsock = false;

    _port = driver::port;
    _first = nullptr;
    _last = nullptr;

    _config.clear();
    _copyright.clear();
    _description.clear();
    _descriptor.clear();
    _display.clear();
    _name.clear();
    _registry.clear();
    _spec.clear();
    _title.clear();
    _usage.clear();

#if defined(_WIN32)
    _hscm = nullptr;
    _lock = nullptr;
    _handle = 0;
    _section = { 0 };
    _status = { 0 };
    _dispatch[0] = { 0 };
    _dispatch[1] = { 0 };
#endif
}

void Driver::Reset()
{
    Init();
}

bool Driver::Stopping()
{
    return _stopping.load(memory_order_acquire);
}

void Driver::Stopping(bool stopping)
{
    _stopping.store(stopping, memory_order_release);
}

bool Driver::Stopped()
{
    return _stopped.load(memory_order_acquire);
}

void Driver::Stopped(bool stopped)
{
    _stopped.store(stopped, memory_order_release);
}

bool Driver::Daemon()
{
    return _daemon.load(memory_order_acquire);
}

void Driver::Daemon(bool daemon)
{
    _daemon.store(daemon, memory_order_release);
}

bool Driver::Winsock()
{
    return _winsock.load(memory_order_acquire);
}

void Driver::Winsock(bool winsock)
{
    _winsock.store(winsock, memory_order_release);
}

void Driver::Config(const char* config)
{
    _config = config;
}

void Driver::Copyright(const char* copyright)
{
    _copyright = copyright;
}

void Driver::Description(const char* description)
{
    _description = description;
}

void Driver::Display(const char* display)
{
    _display = display;
}

void Driver::Name(const char* name)
{
    _name = name;
}

void Driver::Registry(const char* registry)
{
    _registry = registry;
}

void Driver::Spec(const char* spec)
{
    _spec = spec;
}

void Driver::Title(const char* title)
{
    _title = title;
}

void Driver::Usage(const char* usage)
{
    _usage = usage;
}

#if defined(_WIN32)
void Driver::Handle(DWORD control)
{
    //  Handle Windows service control events.

    switch (control) {
    case SERVICE_CONTROL_SHUTDOWN:
    case SERVICE_CONTROL_STOP:

        //  Report the service is stopping.

        EnterCriticalSection(&_section);
        _status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        _status.dwCurrentState = SERVICE_STOP_PENDING;
        _status.dwWin32ExitCode = NO_ERROR;
        _status.dwServiceSpecificExitCode = 0;
        _status.dwCheckPoint = 0;
        _status.dwWaitHint = 0;
        SetServiceStatus(_handle, &_status);
        LeaveCriticalSection(&_section);
        Stopping(true);
        break;
    default:
        EnterCriticalSection(&_section);
        SetServiceStatus(_handle, &_status);
        LeaveCriticalSection(&_section);
    }
}
#endif

#if defined(_WIN32)
void WINAPI ServiceCtrlHandler(DWORD control)
{
    //  Handle Windows service control events.

    theDriver.Handle(control);
}
#endif

#if defined(_WIN32)
void Driver::Main(DWORD argc, LPSTR* argv)
{
    //  Register the service event handler.

    _handle = RegisterServiceCtrlHandler((LPCSTR)_name.c_str(), ServiceCtrlHandler);
    if (!_handle) {
        ExitProcess((UINT)-1);
        return;
    }

    //  Report the service is running.

    InitializeCriticalSection(&_section);
    EnterCriticalSection(&_section);
    _status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _status.dwCurrentState = SERVICE_RUNNING;
    _status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    _status.dwWin32ExitCode = NO_ERROR;
    _status.dwServiceSpecificExitCode = 0;
    _status.dwCheckPoint = 0;
    _status.dwWaitHint = 0;
    SetServiceStatus(_handle, &_status);
    LeaveCriticalSection(&_section);

    //  Apply the service name if provided. Run the program logic on the
    //  service thread. Run will exit when Stopping(true) is called in the
    //  Driver::Handle routine above.

    if (argc) {
        _name = argv[0];
    }
    inf("I%04d %s (%s)", cond::driver::servicestart, cond::driver::message::servicestart, _name.c_str());
    Run(argc, argv);
    inf("I%04d %s (%s)", cond::driver::servicestop, cond::driver::message::servicestop, _name.c_str());

    //  Report the service as stopped.

    EnterCriticalSection(&_section);
    _status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _status.dwCurrentState = SERVICE_STOPPED;
    _status.dwControlsAccepted = 0;
    _status.dwWin32ExitCode = NO_ERROR;
    _status.dwServiceSpecificExitCode = 0;
    _status.dwCheckPoint = 0;
    _status.dwWaitHint = 0;
    SetServiceStatus(_handle, &_status);
    LeaveCriticalSection(&_section);

    //  Delete the critical section resource and exit the service.

    DeleteCriticalSection(&_section);
    ExitProcess(0);
}
#endif

#if defined(_WIN32)
void WINAPI ServiceMain(DWORD argc, LPSTR* argv)
{
    //  Run the program on the service thread.

    theDriver.Main(argc, argv);
}
#endif

#if defined(_WIN32)
bool Driver::OpenServiceManager()
{
    //  Open the service control manager and lock the service database.

    _hscm = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);
    if (!_hscm) {
        OsErr(GetLastError());
        if (ERROR_ACCESS_DENIED == OsErr()) {
            AppErr(cond::driver::access);
            err("E%04d %s", cond::driver::access, cond::driver::message::access);
        } else {
            AppErr(cond::driver::opensc);
            err("E%04d %s (%d)", cond::driver::opensc, cond::driver::message::opensc, OsErr());
        }
        return false;
    }
    _lock = LockServiceDatabase(_hscm);
    if (!_lock) {
        OsErr(GetLastError());
        AppErr(cond::driver::lock);
        err("E%04d %s (%d)", cond::driver::lock, cond::driver::message::lock, OsErr());
        CloseServiceHandle(_hscm);
        return false;
    }
    return true;
}
#endif

#if defined(_WIN32)
void Driver::Install(const char* name)
{
    char* p;
    long result;
    size_t h, pathLen, specLen, pathSpecLen;
    HMODULE module;
    SC_HANDLE hsvc;
    HKEY hkey;
    DWORD disposition;
    SERVICE_DESCRIPTION description;
    char buf[256] = { 0 };
    h = _registry.size() + _name.size() + 1;
    if (h > sizeof(buf)) {
        OsErr(0);
        AppErr(cond::driver::size);
        err("E%04d %s", cond::driver::size, cond::driver::message::size);
        return;
    }
    module = GetModuleHandle(NULL);
    if (!module) {
        OsErr(GetLastError());
        AppErr(cond::driver::module);
        err("E%04d %s (%d)", cond::driver::module, cond::driver::message::module, OsErr());
        return;
    }
    pathLen = GetModuleFileName(module, buf, sizeof(buf));
    if (!pathLen) {
        OsErr(GetLastError());
        AppErr(cond::driver::file);
        err("E%04d %s (%d)", cond::driver::file, cond::driver::message::file, OsErr());
        return;
    }
    for (p = &buf[pathLen]; p > buf;) {
        --p;
        if (('\\' == *p) || ('/' == *p) || (':' == *p)) {
            p++;
            break;
        }
    }
    pathLen = (p - buf);
    specLen = _spec.size();
    pathSpecLen = pathLen + specLen + 2;
    if (pathSpecLen > sizeof(buf)) {
        AppErr(cond::driver::command);
        err("E%04d %s", cond::driver::command, cond::driver::message::command);
        return;
    }
    strcpy(p, _spec.c_str());
    hsvc = CreateService(_hscm, name, _display.c_str(),
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, buf,
        NULL, NULL, NULL, NULL, NULL);
    if (!hsvc) {
        OsErr(GetLastError());
        if (ERROR_SERVICE_EXISTS == OsErr()) {
            AppErr(cond::driver::exists);
            err("E%04d %s (%s)", cond::driver::exists, cond::driver::message::exists, name);
        } else if (ERROR_DUPLICATE_SERVICE_NAME == OsErr()) {
            AppErr(cond::driver::display);
            err("E%04d %s (%s)", cond::driver::display, cond::driver::message::display, name);
        } else {
            AppErr(cond::driver::create);
            err("E%04d %s (%d)", cond::driver::create, cond::driver::message::create, OsErr());
        }
        return;
    }
    description.lpDescription = (LPSTR)_description.c_str();
    ChangeServiceConfig2(hsvc, SERVICE_CONFIG_DESCRIPTION, &description);
    strcpy(buf, _registry.c_str());
    strcat(buf, name);
    result = RegCreateKeyEx(HKEY_LOCAL_MACHINE, buf, 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, &disposition);
    if (result != ERROR_SUCCESS) {
        OsErr(result);
        AppErr(cond::driver::key);
        err("E%04d %s (%d)", cond::driver::key, cond::driver::message::key, OsErr());
        RegCloseKey(hkey);
        CloseServiceHandle(hsvc);
        return;
    }
    result = RegSetValueEx(hkey, "Description", 0, REG_SZ,
        (const BYTE*)_description.c_str(), (DWORD)_description.size() + 1);
    if (result != ERROR_SUCCESS) {
        OsErr(result);
        AppErr(cond::driver::value);
        err("E%04d %s (%d)", cond::driver::value, cond::driver::message::value, OsErr());
        RegCloseKey(hkey);
        CloseServiceHandle(hsvc);
        return;
    }
    result = RegCloseKey(hkey);
    if (result != ERROR_SUCCESS) {
        OsErr(result);
        AppErr(cond::driver::closekey);
        err("E%04d %s (%d)", cond::driver::closekey, cond::driver::message::closekey, OsErr());
        CloseServiceHandle(hsvc);
        return;
    }
    if (!CloseServiceHandle(hsvc)) {
        OsErr(GetLastError());
        AppErr(cond::driver::close);
        err("E%04d %s (%d)", cond::driver::close, cond::driver::message::close, OsErr());
        return;
    }
    inf("I%04d %s (%s)", cond::driver::installed, cond::driver::message::installed, name);
}
#endif

#if defined(_WIN32)
void Driver::Uninstall(const char* name)
{
    SC_HANDLE hsvc = (SC_HANDLE)0;
    hsvc = OpenService(_hscm, name, SERVICE_ALL_ACCESS);
    if (!hsvc) {
        OsErr(GetLastError());
        if (ERROR_SERVICE_DOES_NOT_EXIST == OsErr()) {
            AppErr(cond::driver::missing);
            err("E%04d %s (%s)", cond::driver::missing, cond::driver::message::missing, name);
        } else if (ERROR_INVALID_NAME == OsErr()) {
            AppErr(cond::driver::name);
            err("E%04d %s (%s)", cond::driver::name, cond::driver::message::name, name);
        } else {
            AppErr(cond::driver::open);
            err("E%04d %s (%d)", cond::driver::open, cond::driver::message::open, OsErr());
        }
        return;
    }
    if (!DeleteService(hsvc)) {
        OsErr(GetLastError());
        AppErr(cond::driver::remove);
        err("E%04d %s (%d)", cond::driver::remove, cond::driver::message::remove, OsErr());
        CloseServiceHandle(hsvc);
        return;
    }
    if (!CloseServiceHandle(hsvc)) {
        OsErr(GetLastError());
        AppErr(cond::driver::close);
        err("E%04d %s (%d)", cond::driver::close, cond::driver::message::close, OsErr());
        return;
    }
    inf("I%04d %s (%s)", cond::driver::uninstalled, cond::driver::message::uninstalled, name);
}
#endif

#if defined(_WIN32)
void Driver::CloseServiceManager()
{
    //  Unlock the service database and close the service handle.

    if (!UnlockServiceDatabase(_lock)) {
        if (!AppErr()) {
            OsErr(GetLastError());
            AppErr(cond::driver::unlock);
            err("E%04d %s (%d)", cond::driver::unlock, cond::driver::message::unlock, OsErr());
        }
        CloseServiceHandle(_hscm);
        return;
    }
    if (!CloseServiceHandle(_hscm)) {
        if (!AppErr()) {
            OsErr(GetLastError());
            AppErr(cond::driver::closesc);
            err("E%04d %s (%d)", cond::driver::closesc, cond::driver::message::closesc, OsErr());
        }
    }
}
#endif

#if defined(_WIN32)
BOOL WINAPI ServiceHandleTerm(DWORD fdwCtrlType)
{
    //  The Service method sets ServiceHandleTerm as the closure event handler.
    //  We are on the main thread here. But, we cannot log because theLogger has
    //  already been destructed. We set _stopping to true so the service thread
    //  will exit Mainline and perform finalization.

    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        theDriver.Stopping(true);
        return TRUE;
    default:
        return FALSE;
    }
}
#else
void ServiceHandleTerm(int sig)
{
    //  We are on the child thread. We log that we are stopping, set the
    //  stopping indicator and restore the default handler, then exit.
    //  Control will return to the child thread at the point it was
    //  interrupted and exit Mainline to perform finalization because the
    //  stopping indicator is true.

    inf("I%04d %s", cond::driver::servicestop, cond::driver::message::servicestop);
    theDriver.Stopping(true);
    signal(sig, SIG_DFL);
}
#endif

bool Driver::Service(int argc, char* argv[])
{
#if defined(_WIN32)
    if (argc > 1 && argv[1]) {
        const char* serviceName = _name.c_str();
        if (argc > 2 && argv[2])
            serviceName = argv[2];
        if (!strcmp(argv[1], driver::service)) {
            _dispatch[0].lpServiceName = (char*)serviceName;
            _dispatch[0].lpServiceProc = ServiceMain;
            if (!StartServiceCtrlDispatcher(_dispatch)) {
                LoggerPrelog(false);
                wrt("I%04d %s", cond::driver::title, _title.empty() ? "Title" : _title.c_str());
                wrt("I%04d %s", cond::driver::copyright, _copyright.empty() ? "Copyright" : _copyright.c_str());
                OsErr(GetLastError());
                if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == OsErr()) {
                    AppErr(cond::driver::console);
                    err("E%04d %s", cond::driver::console, cond::driver::message::console);
                } else {
                    AppErr(cond::driver::dispatch);
                    err("E%04d %s (%d)", cond::driver::dispatch, cond::driver::message::dispatch, OsErr());
                }
            } else {
                LoggerConsole(false);
                Daemon(true);
            }
            return false;
        }
        if (!strcmp(argv[1], driver::install)) {
            wrt("I%04d %s", cond::driver::title, _title.empty() ? "Title" : _title.c_str());
            wrt("I%04d %s", cond::driver::copyright, _copyright.empty() ? "Copyright" : _copyright.c_str());
            wrt("I%04d %s (%s)", cond::driver::installing, cond::driver::message::installing, serviceName);
            if (OpenServiceManager()) {
                Install(serviceName);
                CloseServiceManager();
            }
            return false;
        }
        if (!strcmp(argv[1], driver::uninstall)) {
            wrt("I%04d %s", cond::driver::title, _title.empty() ? "Title" : _title.c_str());
            wrt("I%04d %s", cond::driver::copyright, _copyright.empty() ? "Copyright" : _copyright.c_str());
            wrt("I%04d %s (%s)", cond::driver::uninstalling, cond::driver::message::uninstalling, serviceName);
            if (OpenServiceManager()) {
                Uninstall(serviceName);
                CloseServiceManager();
            }
            return false;
        }
        wrt("I%04d %s", cond::driver::usage, _usage.empty() ? "Usage" : _usage.c_str());
        return false;
    }
    SetConsoleCtrlHandler(ServiceHandleTerm, TRUE);
#else
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    int rc = sigaction(SIGPIPE, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::pipe1);
        err("E%04d %s (%d)", cond::driver::pipe1, cond::driver::message::pipe1, OsErr());
        return false;
    }
    rc = sigaction(SIGALRM, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::alarm1);
        err("E%04d %s (%d)", cond::driver::alarm1, cond::driver::message::alarm1, OsErr());
        return false;
    }
    if ((argc < 2) || (strcmp(driver::service, argv[1]))) {
        memset(&act, 0, sizeof(act));
        act.sa_handler = ServiceHandleTerm;
        act.sa_flags = 0;
        rc = sigaction(SIGINT, &act, NULL);
        if (rc) {
            OsErr(errno);
            AppErr(cond::driver::intr);
            err("E%04d %s (%d)", cond::driver::intr, cond::driver::message::intr, OsErr());
            return false;
        }
        return true;
    }
    Daemon(true);
    rc = fork();
    if (-1 == rc) {
        OsErr(errno);
        AppErr(cond::driver::fork);
        err("E%04d %s (%d)", cond::driver::fork, cond::driver::message::fork, OsErr());
        return false;
    }
    if (rc)
        exit(0);
    rc = setsid();
    if (-1 == rc) {
        OsErr(errno);
        AppErr(cond::driver::setsid);
        err("E%04d %s (%d)", cond::driver::setsid, cond::driver::message::setsid, OsErr());
        return false;
    }
    sigset_t set;
    rc = sigemptyset(&set);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::emptyset);
        err("E%04d %s (%d)", cond::driver::emptyset, cond::driver::message::emptyset, OsErr());
        return false;
    }
    rc = sigaddset(&set, SIGINT);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::addset);
        err("E%04d %s (%d)", cond::driver::addset, cond::driver::message::addset, OsErr());
        return false;
    }
    rc = sigprocmask(SIG_UNBLOCK, &set, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::procmask);
        err("E%04d %s (%d)", cond::driver::procmask, cond::driver::message::procmask, OsErr());
        return false;
    }
    memset(&act, 0, sizeof(act));
    act.sa_handler = ServiceHandleTerm;
    act.sa_flags = 0;
    rc = sigaction(SIGINT, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::term);
        err("E%04d %s (%d)", cond::driver::term, cond::driver::message::term, OsErr());
        return false;
    }
    memset(&act, 0, sizeof(act));
    act.sa_handler = SIG_IGN;
    act.sa_flags = 0;
    rc = sigaction(SIGPIPE, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::pipe2);
        err("E%04d %s (%d)", cond::driver::pipe2, cond::driver::message::pipe2, OsErr());
        return false;
    }
    rc = sigaction(SIGALRM, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::alarm2);
        err("E%04d %s (%d)", cond::driver::alarm2, cond::driver::message::alarm2, OsErr());
        return false;
    }
    rc = sigaction(SIGHUP, &act, NULL);
    if (rc) {
        OsErr(errno);
        AppErr(cond::driver::hup);
        err("E%04d %s (%d)", cond::driver::hup, cond::driver::message::hup, OsErr());
        return false;
    }
    int nullFile = ::open("/dev/null", O_RDWR);
    if (-1 == nullFile) {
        OsErr(errno);
        AppErr(cond::driver::null);
        err("E%04d %s (%d)", cond::driver::null, cond::driver::message::null, OsErr());
        return false;
    }
    rc = dup2(nullFile, STDIN_FILENO);
    if (-1 == rc) {
        OsErr(errno);
        AppErr(cond::driver::stdin_);
        err("E%04d %s (%d)", cond::driver::stdin_, cond::driver::message::stdin_, OsErr());
        return false;
    }
    rc = dup2(nullFile, STDOUT_FILENO);
    if (-1 == rc) {
        OsErr(errno);
        AppErr(cond::driver::stdout_);
        err("E%04d %s (%d)", cond::driver::stdout_, cond::driver::message::stdout_, OsErr());
        return false;
    }
    rc = dup2(nullFile, STDERR_FILENO);
    if (-1 == rc) {
        OsErr(errno);
        AppErr(cond::driver::stderr_);
        err("E%04d %s (%d)", cond::driver::stderr_, cond::driver::message::stderr_, OsErr());
        return false;
    }
#endif
    return true;
}

#if defined(_WIN32)
bool Driver::GetRegistryValue(HKEY key, const char* name, char* buf, DWORD* size)
{
    DWORD type = 0;
    long result = RegGetValue(key, NULL, name, RRF_RT_REG_SZ, &type, buf, size);
    if (result == ERROR_SUCCESS)
        return true;
    if (result == ERROR_FILE_NOT_FOUND) {
        *size = 0;
        return true;
    }
    OsErr(result);
    AppErr(cond::driver::reggetvalue);
    err("E%04d %s (%d)", cond::driver::reggetvalue, cond::driver::message::reggetvalue, result);
    RegCloseKey(key);
    return false;
}
#endif

#if defined(_WIN32)
void Driver::GetRegistryVars()
{
    HKEY key = 0;
    std::string path = _registry + _name;
    long result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, path.c_str(), 0, KEY_QUERY_VALUE, &key);
    if (result != ERROR_SUCCESS) {
        if (result != ERROR_FILE_NOT_FOUND) {
            OsErr(result);
            AppErr(cond::driver::regopenkeyex);
            err("E%04d %s (%d)", cond::driver::regopenkeyex, cond::driver::message::regopenkeyex, OsErr());
        }
        return;
    }
    char val[256]{ 0 };
    DWORD size = sizeof val;
    if (!GetRegistryValue(key, driver::setting::name::descriptor, val, &size))
        return;
    if (size) {
        inf(cond::driver::message::regsetting, cond::driver::regsetting, driver::setting::name::descriptor, val);
        _descriptor = val;
    }
    size = sizeof val;
    if (!GetRegistryValue(key, driver::setting::name::loglevel, val, &size))
        return;
    if (size) {
        inf(cond::driver::message::regsetting, cond::driver::regsetting, driver::setting::name::loglevel, val);
        LoggerLevel(val);
    }
    size = sizeof val;
    if (!GetRegistryValue(key, driver::setting::name::logpath, val, &size))
        return;
    if (size) {
        inf(cond::driver::message::regsetting, cond::driver::regsetting, driver::setting::name::logpath, val);
        LoggerPath(val);
    }
    size = sizeof val;
    if (!GetRegistryValue(key, driver::setting::name::logprefix, val, &size))
        return;
    if (size) {
        inf(cond::driver::message::regsetting, cond::driver::regsetting, driver::setting::name::logprefix, val);
        LoggerPrefix(val);
    }
    size = sizeof val;
    if (!GetRegistryValue(key, driver::setting::name::logsuffix, val, &size))
        return;
    if (size) {
        inf(cond::driver::message::regsetting, cond::driver::regsetting, driver::setting::name::logsuffix, val);
        LoggerSuffix(val);
    }
    result = RegCloseKey(key);
    if (result != ERROR_SUCCESS) {
        OsErr(result);
        AppErr(cond::driver::regclosekey);
        err("E%04d %s (%d)", cond::driver::regclosekey, cond::driver::message::regclosekey, result);
    }
}
#endif

void Driver::GetEnvVars()
{
    const char* val = getenv(driver::env::descriptor);
    if (val) {
        inf(cond::driver::message::envsetting, cond::driver::envsetting, driver::setting::name::descriptor, val);
        LoggerLevel(val);
    }
    val = getenv(driver::env::loglevel);
    if (val) {
        inf(cond::driver::message::envsetting, cond::driver::envsetting, driver::setting::name::loglevel, val);
        LoggerLevel(val);
    }
    val = getenv(driver::env::logpath);
    if (val) {
        inf(cond::driver::message::envsetting, cond::driver::envsetting, driver::setting::name::logpath, val);
        LoggerPath(val);
    }
    val = getenv(driver::env::logprefix);
    if (val) {
        inf(cond::driver::message::envsetting, cond::driver::envsetting, driver::setting::name::logprefix, val);
        LoggerPrefix(val);
    }
    val = getenv(driver::env::logsuffix);
    if (val) {
        inf(cond::driver::message::envsetting, cond::driver::envsetting, driver::setting::name::logsuffix, val);
        LoggerSuffix(val);
    }
}

void Driver::GetFileArg(int argc, char* argv[])
{
    for (int arg = 1; arg < argc; arg++) {
        if (!strcmp(argv[arg], driver::arg::configfile)) {
            if (++arg > argc)
                continue;
            inf(cond::driver::message::argsetting, cond::driver::argsetting, driver::setting::name::configfile, argv[arg]);
            Config(argv[arg]);
        }
    }
}

void Driver::SetVar(unsigned int id, const char* val)
{
    switch (id) {
    case driver::setting::configfile:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::configfile, val);
        _config = val;
        break;
    case driver::setting::descriptor:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::descriptor, val);
        _descriptor = val;
        break;
    case driver::setting::loglevel:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::loglevel, val);
        LoggerLevel(val);
        break;
    case driver::setting::logpath:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::logpath, val);
        LoggerPath(val);
        break;
    case driver::setting::logprefix:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::logprefix, val);
        LoggerPrefix(val);
        break;
    case driver::setting::logsuffix:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::logsuffix, val);
        LoggerSuffix(val);
        break;
    case driver::setting::model:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::model, val);
        _model = val;
        break;
    case driver::setting::port:
        inf(cond::driver::message::cfgsetting, cond::driver::cfgsetting, driver::setting::name::port, val);
        int port_n = atoi(val);
        port_n = port_n < 0 ? 0 : port_n > 65535 ? 65535 : port_n;
        _port = static_cast<uint16_t>(port_n);
        break;
    }
}

void Driver::ParseLine(std::string& line)
{
    std::string setting;
    std::string value;
    size_t loc = line.find_first_not_of(" \t");
    if (loc == std::string::npos)
        return;
    line.erase(0, loc);
    loc = line.find_first_of("=");
    if (loc == std::string::npos)
        return;
    setting = line.substr(0, loc);
    value = line.substr(loc);
    loc = setting.find_first_of(" \t");
    if (loc != std::string::npos)
        setting.erase(loc);
    loc = value.find_first_not_of("= \t");
    if (loc == std::string::npos)
        return;
    value.erase(0, loc);
    loc = value.find_first_of(" \t");
    if (loc != std::string::npos)
        value.erase(loc);
    for (int n = 0; app_settings[n].id; n++) {
        if (setting == app_settings[n].str) {
            SetVar(app_settings[n].id, value.c_str());
            break;
        }
    }
}

void Driver::GetFileVars()
{
    ifstream file(_config, ios::in);
    if (file.good()) {
        string str;
        while (getline(file, str)) {
            ParseLine(str);
        }
        file.close();
    }
}

void Driver::GetArgVars(int argc, char* argv[])
{
    if (argc < 2)
        return;
    int n = 1;
    if (!strcmp(argv[n], driver::service))
        n++;
    while (n < argc) {
        if (!strcmp(argv[n], driver::arg::configfile)) {
            if (++n < argc) {
                Config(argv[n]);
            }
        } else if (!strcmp(argv[n], driver::arg::descriptor)) {
            if (++n < argc) {
                _descriptor = argv[n];
            }
        } else if (!strcmp(argv[n], driver::arg::loglevel)) {
            if (++n < argc) {
                LoggerLevel(argv[n]);
            }
        } else if (!strcmp(argv[n], driver::arg::logpath)) {
            if (++n < argc) {
                LoggerPath(argv[n]);
            }
        } else if (!strcmp(argv[n], driver::arg::logprefix)) {
            if (++n < argc) {
                LoggerPrefix(argv[n]);
            }
        } else if (!strcmp(argv[n], driver::arg::logsuffix)) {
            if (++n < argc) {
                LoggerSuffix(argv[n]);
            }
        } else if (!strcmp(argv[n], driver::arg::port)) {
            if (++n < argc) {
                int port_n = std::atoi(argv[n]);
                port_n = port_n < 0 ? 0 : port_n > 65535 ? 65535 : port_n;
                _port = static_cast<uint16_t>(port_n);
            }
        }
        n++;
    }
}

bool Driver::GetModel()
{
    //  If our configuration included value for the "descriptor" then we need to
    //  read this file, confirm it is valid JSON and apply it to all of our
    //  channels.

    if (!_descriptor.empty()) {
        ifstream descriptor(_descriptor, ifstream::in);
        if (descriptor.good() && descriptor.is_open()) {
            ostringstream ss;
            ss << descriptor.rdbuf();
            _model = ss.str();
            if (!_json.Parse(_model))
                return false;
            inf("I%04d %s", cond::driver::modelloaded, cond::driver::message::modelloaded);
            _udpChannel4.SetJson(_json);
            _udpChannel6.SetJson(_json);
            for (auto& c : _channel) {
                c.SetJson(_json);
            }
        }
    }
    return true;
}

void Driver::Configure(int argc, char* argv[])
{
    LoggerPrefix(_name.c_str());
    LoggerSuffix(driver::logsuffix);
#if defined(_WIN32)
    GetRegistryVars();
#endif
    GetEnvVars();
    GetFileArg(argc, argv);
    GetFileVars();
    GetArgVars(argc, argv);
    GetModel();
    LoggerPrelog(false);
    inf("I%04d %s", cond::driver::complete, cond::driver::message::complete);
}

bool Driver::OpenAddress(Socket& tcp, Socket& udp, bool v6)
{
    if (!tcp.Stream(v6))
        return false;
    if (!tcp.Bind())
        return false;
    if (!tcp.Listen())
        return false;
    if (!udp.Datagram(v6))
        return false;
    return udp.Bind();
}

void Driver::CloseAddress(Socket& tcp, Socket& udp)
{
    tcp.Shutdown();
    tcp.Close();
    udp.Close();
}

bool Driver::Initialize(int argc, char* argv[])
{
    wrt("I%04d %s", cond::driver::title, _title.c_str());
    wrt("I%04d %s", cond::driver::copyright, _copyright.c_str());
#if defined(_WIN32)
    WSADATA wsadata{ 0 };
    int rc = WSAStartup(WINSOCKVERSION, &wsadata);
    if (rc) {
        AppErr(cond::driver::wsastartup);
        return false;
    }
    Winsock(true);
#endif
    Configure(argc, argv);
    _v4tcp.Port(_port);
    _v6tcp.Port(_port);
    _udpChannel4.Sock().Port(_port);
    _udpChannel6.Sock().Port(_port);
    if (!OpenAddress(_v4tcp, _udpChannel4.Sock()))
        return false;
    if (!OpenAddress(_v6tcp, _udpChannel6.Sock(), true))
        return false;
    return true;
}

void Driver::Queue(Channel* channel)
{
    if (channel) {
        channel->Next(nullptr);
        channel->Prev(_last);
        if (_last)
            _last->Next(channel);
        _last = channel;
        if (!_first)
            _first = channel;
    }
}

void Driver::GetClient()
{
    int n = 0;
    for (; n < driver::channels; n++) {
        if (_channel[n].State() == channel::state::ready) {
            SOCKET client = _v4tcp.Accept();
            if (client) {
                _channel[n].SetAddressAndPortFrom(_v4tcp);
            } else {
                client = _v6tcp.Accept();
                if (client) {
                    _channel[n].SetAddressAndPortFrom(_v6tcp);
                }
            }
            if (client) {
                _channel[n].State(channel::state::needfirstbytes);
                _channel[n].Sock(client);
                _channel[n].Expires(std::time(0) + channel::expires);
                Queue(&_channel[n]);
            }
            break;
        }
    }

    //  If all channels are busy, accept but close.

    if (n >= driver::channels) {
        SOCKET client = _v4tcp.Accept();
        if (client) {
            Channel channel;
            channel.SetAddressAndPortFrom(_v4tcp);
            channel.Close();
        }
    }
}

Channel* Driver::Dequeue()
{
    Channel* channel = _first;
    if (channel) {
        if (channel->Prev())
            channel->Prev()->Next(channel->Next());
        if (channel->Next())
            channel->Next()->Prev(channel->Prev());
        _first = channel->Next();
        if (channel == _last)
            _last = channel->Prev();
    }
    return channel;
}

void Driver::ServiceChannel()
{
    //_udpChannel4.Service();
    //_udpChannel6.Service();
    Channel* channel = Dequeue();
    if (channel) {
        channel->Service();
        if (channel->State() != channel::state::ready)
            Queue(channel);
    }
}

void Driver::Mainline()
{
    //  Accept incoming connections and service the channels until stopping.

    while (!Stopping()) {
        GetClient();
        ServiceChannel();
    }

    inf("I%04d %s", cond::driver::stopping, cond::driver::message::stopping);
}

void Driver::Finalize()
{
    //  Shutdown and close TCP and UDP listeners on both IPv4 and IPv6.

    CloseAddress(_v4tcp, _udpChannel4.Sock());
    CloseAddress(_v6tcp, _udpChannel6.Sock());

    //  Close any open channel sockets.

    for (auto &c : _channel) {
        c.Close();
    }

#if defined(_WIN32)
    //  Cleanup Windows Sockets resources if WSAStartup succeeded.

    if (Winsock()) {
        WSACleanup();
        Winsock(false);
    }
#endif

    inf("I%04d %s", cond::driver::finalize, cond::driver::message::finalize);
}

void Driver::Run(int argc, char* argv[])
{
    //  Enter the Driver's mainline loop only if initialization succeeds. Call
    //  Finalize and indicate thread completion in any case.

    if (Initialize(argc, argv)) {
        Mainline();
    }
    Finalize();
    Stopped(true);
}

bool Driver::Start(int argc, char* argv[])
{
    //  Perform install, uninstall or start the Windows service. If an install
    //  or uninstall was run or the Windows service dispatcher was called, then
    //  we can exit now.

    if (!Service(argc, argv)) {
        return false;
    }

    //  If Service returned true, we continue. Now if the "service" argument was
    //  passed on Linux or macOS, we are already forked and can enter the Run
    //  method on this task and then return false since there is no console.

    if (Daemon()) {
        Run(argc, argv);
        return false;
    }

    //  If Service returned true but we not a daemon, start a background thread
    //  and call Run on it. Then return true to accept console input.

    _thread = std::thread([=]() {
        Run(argc, argv);
    });
    return true;
}

void Driver::Stop()
{
    //  If we reach Stop, then no error has occurred. Clear the application
    //  error code and set the stopping indicator.

    AppErr(0);
    inf("I%04d %s", cond::driver::servicestop, cond::driver::message::servicestop);
    Stopping(true);

    //  Wait up to 1 second for the thread finish finalization.

    for (int n = driver::stopwait; n && !Stopped(); n--) {
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    //  If Finalization finished, join the thread if joinable.

    if (Stopped() && _thread.joinable()) {
        _thread.join();
    }
}

int Driver::Result()
{
    return AppErr();
}

IDriver& TheDriver()
{
    return theDriver;
}

IDriver* TheDriverPtr()
{
    return &theDriver;
}
