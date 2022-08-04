#pragma once

#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>

namespace terra
{
struct Time
{
    static constexpr int DAY_DURATION = 60 * 60 * 24;
};

using TimeInterval = std::array<std::tm, 2>;

inline int tm_to_seconds(const std::tm& src)
{
    return src.tm_hour * 60 * 60 + src.tm_min * 60 + src.tm_sec;
}

inline bool str_to_time(const std::string& str, int& dest, const char* format = "%H:%M")
{
    std::tm tm{};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, format);
    if (ss.fail()) {
        return false;
    }

    dest = tm_to_seconds(tm);

    return true;
}

inline std::string time_to_str(const std::tm& tm)
{
    return fmt::format("{}-{:02d}-{:02d}T{:02d}:{:02d}:{:02d}+00:00", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
}
