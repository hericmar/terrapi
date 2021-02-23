#pragma once

#include "fmt/format.h"

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);
#define LOG_DEBUG(...) printf(__VA_ARGS__);

namespace Log
{
template <typename... Args>
void Error(const std::string& fmt, Args&&... args)
{
    fprintf(stderr, "Error: ");
    fprintf(stderr, fmt::format(fmt, std::forward<Args>(args)...).c_str());
    fprintf(stderr, "\n");

    std::cout << std::flush;
}

template <typename... Args>
void Warning(const std::string& fmt, Args&&... args)
{
    printf(fmt::format(fmt, std::forward<Args>(args)...).c_str());
    printf("\n");

    std::cout << std::flush;
}

template <typename... Args>
void Info(const std::string& fmt, Args&&... args)
{
    printf(fmt::format(fmt, std::forward<Args>(args)...).c_str());
    printf("\n");

    std::cout << std::flush;
}
} // namespace Log
