#pragma once

#include <ctime>
#include <iomanip>
#include <string>

namespace Terra
{
typedef float Time;

inline std::tm StringToCTime(const std::string& str, const char* format)
{
    struct std::tm tm{};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, format);

    return tm;
}

inline Time TimeToFloat(std::tm& time)
{
    return (float) time.tm_hour / 24.0f + (1.0f / 24.0f) * ((float) time.tm_min / 60.0f);
}

inline Time StringToTime(const std::string& str, const char* format = "%H:%M")
{
    auto time = StringToCTime(str, format);
    return TimeToFloat(time);
}
}
