#include <cstdio>

#include "fmt/format.h"

namespace terra
{
enum LogLevel
{
    DEBUG,
    TRACE,
    INFO,
    WARN,
    ERR,
    FATAL,
};

namespace detail
{
std::string iso_now_str();
const char* level_str(LogLevel level);
bool should_log(LogLevel level);
}

#define LOG(level, ...) if (detail::should_log(level)) { fprintf(stderr, "%s: %s: %s\n", detail::iso_now_str().c_str(), detail::level_str(level), fmt::format(__VA_ARGS__).c_str()); }
}
