#pragma once

#include <ctime>
#include <cstdlib>
#include <optional>
#include <string>

/// @todo How to represent "each second day"

namespace terra
{
using Time = uint64_t;

std::optional<Time> parse_time_from_str(const std::string& str, const char* format = "%H:%M");

Time to_seconds(const std::tm& tm);
}
