#include "utils.h"

namespace terra
{
std::optional<uint64_t> parse_time_from_str(const std::string& str, const char* format)
{
    std::tm result;

    std::istringstream ss(str);
    ss >> std::get_time(&result, format);
    if (ss.fail()) {
        return std::nullopt;
    }

    if (result.tm_sec <= 0) {
        result.tm_sec = 0;
    }

    return to_seconds(result);
}

uint64_t to_seconds(const std::tm& tm)
{
    return tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;
}
}
