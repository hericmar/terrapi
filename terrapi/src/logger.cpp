#include "logger.h"

#include <array>
#include <stdio.h>
#include <time.h>

namespace terra
{
void log_message(const char* level, const std::string& message)
{
    log_message(level, message.c_str());
}

void log_message(const char* level, const std::string_view& message)
{
    log_message(level, std::string(message.begin(), message.end()));
}

void log_message(const char* level, const char* message)
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

    fprintf(stderr, "%s: %s: %s\n", datetime_buffer, level, message);
}
}