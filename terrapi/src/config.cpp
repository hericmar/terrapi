#include <optional>
#include <map>

#include "terrapi/terrapi.h"

#include "config.h"

namespace terra
{
using Lines = std::vector<std::string_view>;
using Map = std::map<std::string, Lines>;

/// https://www.cppstories.com/2018/07/string-view-perf-followup/
static Lines split(std::string_view str, std::string_view delims = " ")
{
    std::vector<std::string_view> output;
    size_t first = 0;

    while (first < str.size())
    {
        const auto second = str.find_first_of(delims, first);

        if (first != second)
            output.emplace_back(str.substr(first, second-first));

        if (second == std::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}

static void parse_environment(Context& ctx, const Lines& lines)
{

}

struct Section
{
    std::string_view type;
    std::string_view name;
};

void parse_config(Context& ctx, std::string_view str)
{
    ctx.m_switches.clear();

    const auto lines = split(str, "\n");
    const auto l = lines;

    for (int i = 0; i < lines.size(); ++i)
    {
        if ()
    }
}
}
