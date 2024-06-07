#include "api.h"
#include "logger.h"

#include <fstream>
#include <iostream>

using namespace std;

namespace logger {

    namespace max {
        enum {
            size = 512
        };
    }

    namespace level {
        namespace name {
            constexpr const char* error = "error";
            constexpr const char* warning = "warning";
            constexpr const char* info = "info";
            constexpr const char* summary = "summary";
            constexpr const char* detail = "detail";
            constexpr const char* trace = "trace";
            constexpr const char* debug = "debug";
        }
    }

    typedef struct levelname {
        const char* _name;
        unsigned int _level;
    } LEVELNAME;

    const LEVELNAME levels[] = {
        {logger::level::name::error, logger::level::error},
        {logger::level::name::warning, logger::level::warning},
        {logger::level::name::info, logger::level::info},
        {logger::level::name::summary, logger::level::summary},
        {logger::level::name::detail, logger::level::detail},
        {logger::level::name::trace, logger::level::trace},
        {logger::level::name::debug, logger::level::debug},
        {0,0}
    };

    //  Log information written to files may be directed to specified path. The
    //  log file names can consist of a prefix, base, and suffix, delimited by
    //  periods. If unspecified, the base is the date in yyyymmdd format. The
    //  prefix is usually set to the program name during initialization.

    constexpr const char* base = "";
    constexpr const char* path = "";
    constexpr const char* prefix = "";
    constexpr const char* prelog = "prelog";
    constexpr const char* suffix = "log";
}

//  There is only ever one one instance of the Logger. It is constructed when
//  the library is loaded, before control reaches main. It is destructed when
//  the library is unloaded. No reference to the Logger is ever returnd by the
//  library. Instead, public APIs are defined such as LoggerLevel, etc.

Logger theLogger;

Logger::Logger()
{
    //  Initialize members to default values.

    Init();
}

Logger::~Logger()
{
    Reset();
}

void Logger::Init()
{
    //  Initialize members to default values. This is only ever called when the
    //  singleton is created, before main() is called. So simple assignments
    //  can be used here.

    _appErr = 0;
    _osErr = 0;
    _level = logger::level::info;

    _console = true;
    _prelog = true;

    _path = logger::path;
    _prefix = logger::prefix;
    _base = logger::base;
    _suffix = logger::suffix;
}

void Logger::Reset()
{
    Init();
}

int Logger::AppErr()
{
    return _appErr.load(memory_order_acquire);
}

void Logger::AppErr(int appErr)
{
    _appErr.store(appErr, memory_order_release);
}

int Logger::OsErr()
{
    return _osErr.load(memory_order_acquire);
}

void Logger::OsErr(int osErr)
{
    _osErr.store(osErr, memory_order_release);
}

unsigned int Logger::Level()
{
    return _level.load(memory_order_acquire);
}

bool Logger::Level(unsigned int level)
{
    _level.store(level > logger::level::debug ? logger::level::debug : level, memory_order_release);
    return true;
}

bool Logger::Console()
{
    lock_guard<mutex> lock(_mutex);
    return _console;
}

void Logger::Console(bool console)
{
    lock_guard<mutex> lock(_mutex);
    _console = console;
}

bool Logger::Prelog()
{
    lock_guard<mutex> lock(_mutex);
    return _prelog;
}

void Logger::Prelog(bool prelog)
{
    lock_guard<mutex> lock(_mutex);
    _prelog = prelog;
}

void Logger::Path(const char* path)
{
    lock_guard<mutex> lock(_mutex);
    _path = path;
}

void Logger::Prefix(const char* prefix)
{
    lock_guard<mutex> lock(_mutex);
    _prefix = prefix;
}

void Logger::Base(const char* base)
{
    lock_guard<mutex> lock(_mutex);
    _base = base;
}

void Logger::Suffix(const char* suffix)
{
    lock_guard<mutex> lock(_mutex);
    _suffix = suffix;
}

void Logger::Write(const char* msg)
{
    if (!msg)
        return;

    time_t t = time(0);
    tm* tm = gmtime(&t);
    if (!tm)
        return;

    char filetime[9]{};
    char datetime[21]{};
    strftime(filetime, sizeof filetime, "%Y%m%d", tm);
    strftime(datetime, sizeof datetime, "%Y.%m.%d %H:%M:%S ", tm);

    lock_guard<mutex> lock(_mutex);
    if (_console)
        cout << msg << endl;

    if (_prelog) {
        ofstream prelog(logger::prelog, ofstream::app);
        if (prelog.is_open()) {
            prelog << datetime << msg << endl;
            prelog.close();
        }
        return;
    }

    string spec;
    if (!_path.empty())
        spec = _path + "/";
    if (!_prefix.empty())
        spec += _prefix + ".";
    if (!_base.empty())
        spec += _base;
    else
        spec += filetime;
    if (!_suffix.empty())
        spec += "." + _suffix;

    ofstream logfile(spec, ofstream::app);
    if (logfile.is_open()) {
        ifstream prelog(logger::prelog, ofstream::in);
        if (prelog.is_open()) {
            logfile << prelog.rdbuf();
            prelog.close();
            remove(logger::prelog);
        }
        logfile << datetime << msg << endl;
        logfile.close();
    }
}

int AppErr()
{
    return theLogger.AppErr();
}

void AppErr(int appErr)
{
    theLogger.AppErr(appErr);
}

int OsErr()
{
    return theLogger.OsErr();
}

void OsErr(int osErr)
{
    theLogger.OsErr(osErr);
}

bool LoggerLevel(const char* level)
{
    const logger::LEVELNAME* lln = &logger::levels[0];
    for (; lln->_name && strcmp(level, lln->_name); lln++);
    return (lln->_name ? theLogger.Level(lln->_level) : false);
}

void LoggerConsole(bool console)
{
    theLogger.Console(console);
}

void LoggerPrelog(bool prelog)
{
    theLogger.Prelog(prelog);
}

void LoggerPath(const char* path)
{
    theLogger.Path(path);
}

void LoggerPrefix(const char* prefix)
{
    theLogger.Prefix(prefix);
}

void LoggerBase(const char* base)
{
    theLogger.Base(base);
}

void LoggerSuffix(const char* suffix)
{
    theLogger.Suffix(suffix);
}

void LoggerWrite(unsigned int level, const char* fmt, ...)
{
    if (theLogger.Level() >= level) {
        va_list arglist;
        va_start(arglist, fmt);
        char msg[logger::max::size]{ 0 };
        vsnprintf(msg, sizeof(msg) - 1, fmt, arglist);
        va_end(arglist);
        theLogger.Write(msg);
    }
}
