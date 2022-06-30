#pragma once

#include <cstdio>
#include <string_view>

#include "fmt/format.h"

namespace terra
{
namespace fs
{
    bool exists(const char* path);

    std::string read(const char* path);
}

namespace log
{
    template <typename... Args>
    void info(const std::string& str, Args&&... args)
    {
        fprintf(stderr, "%s\n", fmt::format(str, std::forward<Args>(args)...).c_str());
    }

    template <typename... Args>
    void warn(const std::string& str, Args&&... args)
    {
        fprintf(stderr, "%s\n", fmt::format(str, std::forward<Args>(args)...).c_str());
    }

    template <typename... Args>
    void err(const std::string& str, Args&&... args)
    {
        fprintf(stderr, "%s\n", fmt::format(str, std::forward<Args>(args)...).c_str());
    }
}
}
