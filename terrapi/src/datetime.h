#pragma once

#include <ctime>
#include <optional>
#include <string_view>

/// @todo How to represent "each second day"

namespace terra
{
using Time = unsigned;

std::optional<Time> parse_time_from_str(const std::string& str, const char* format = "%H:%M");

Time to_seconds(const std::tm& tm);
}
