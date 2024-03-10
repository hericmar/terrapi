// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#pragma once

#include <charconv>
#include <optional>
#include <ctime>
#include <cctype>
#include <string>
#include <vector>
#include <iomanip>

namespace terra
{
inline bool is_str_literal(int c)
{
    return std::isalnum(c) || c == '.';
}

inline bool is_time_char(int c)
{
    return std::isalnum(c) || c == ':';
}

inline std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> result;
    std::string s = str;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        result.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    result.push_back(s);

    return result;
}

/// Returns seconds withing the day!
std::optional<uint64_t> parse_time_from_str(const std::string& str, const char* format = "%H:%M");

uint64_t to_seconds(const std::tm& tm);
}
