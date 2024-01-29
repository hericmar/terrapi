#include "logger.h"

#include <stdio.h>
#include <time.h>

namespace terra
{
static auto log_level = TRACE;
static const char* LOG_LEVEL_STR[] = {
    "debug",
    "trace",
    "info",
    "warn",
    "error",
    "fatal",
};

namespace detail
{
std::string iso_now_str()
{
    time_t t = time(nullptr);
    tm*    now  = localtime(&t);

    char datetime_buffer[sizeof "2011-11-11 11:01:01"];
    sprintf(datetime_buffer, "%04d-%02d-%02d %02d:%02d:%02d",
            now->tm_year + 1900,
            now->tm_mon + 1,
            now->tm_mday,
            now->tm_hour,
            now->tm_min,
            now->tm_sec);

    return std::string(datetime_buffer);
}

const char* level_str(LogLevel level)
{
    return LOG_LEVEL_STR[level];
}

bool should_log(LogLevel level)
{
    return level >= log_level;
}
}
}