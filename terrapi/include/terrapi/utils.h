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

namespace string_utils
{
    inline std::string replace(std::string str, const std::string& from, const std::string& to)
    {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
        }
        return str;
    }
}
}
