#include <array>
#include <charconv>
#include <optional>
#include <map>

#include "terrapi/terrapi.h"

#include "config.h"

namespace terra
{
using Lines = std::vector<std::string_view>;

struct Section
{
    std::string type;
    std::string name;
};

struct SectionComparator
{
    bool operator()(const Section& lhs, const Section& rhs) const {
        return lhs.type < rhs.type || (!(rhs.type < lhs.type) && lhs.name < rhs.name);
    }
};

using SectionBody = std::map<std::string, std::pair<std::string, int>>;  // key, { val, line }

using ConfigMap = std::map<
    Section,
    SectionBody,
    SectionComparator
>;

static const std::map<std::string, bool> g_sections = {
    {"environment", false},
    {"sensor", true},
    {"switch", true},
    {"timer", true}
};

static int line_number = 0;

/// Lines with breaks are empty.
///
/// https://www.cppstories.com/2018/07/string-view-perf-followup/
static Lines split(std::string_view str, std::string_view delims = " ")
{
    std::vector<std::string_view> output;
    size_t first = 0;

    if (str.size() < 2) {
        return output;
    }

    while (first < str.size())
    {
        const auto second = str.find_first_of(delims, first);

        if (first != second) {
            output.emplace_back(str.substr(first, second - first));
        }
        else {
            // push empty lines
            size_t next = first - 1;

            if (output.size() > 2) {
                if (output.back()[0] != '\n' && output[output.size() - 2][0] == '\n') {
                    // left empty
                } else {
                    output.emplace_back(str.substr(next, 1));
                }
            }
        }

        if (second == std::string_view::npos)
            break;

        first = second + 1;
    }

    return output;
}

static constexpr bool is_whitespace(char c) {
    return c == ' ' || c == '\n';
}

static std::string sanitize_line(std::string_view line)
{
    std::vector<char> sanitized;
    sanitized.reserve(line.size());

    for (char c : line) {
        if (is_whitespace(c)) {
            continue;
        }
        sanitized.push_back(c);
    }
    sanitized.push_back('\0');

    if (!sanitized.empty() && sanitized[0] == '#') {
        return "";
    }

    return sanitized.data();
}

static bool starts_with(std::string_view str, std::string_view other)
{
    if (other.size() > str.size()) {
        return false;
    }

    for (int i = 0; i < other.size(); ++i) {
        if (str[i] != other[i]) {
            return false;
        }
    }

    return true;
}

static std::optional<Section> get_section(std::string_view sanitized_line)
{
    if (sanitized_line.size() < 2) {
        // too short for a section
        return std::nullopt;
    }

    if (sanitized_line[0] != '[' || sanitized_line[sanitized_line.size() - 1] != ']') {
        // not a section
        return std::nullopt;
    }

    std::string_view section;
    for (const auto& [section_type, has_name] : g_sections) {
        if (starts_with(sanitized_line.substr(1, sanitized_line.size() - 2), section_type)) {
            section = section_type;
            break;
        }
    }

    if (section.empty()) {
        log::err("Unknown section \"{}\" at line {}.", section, line_number);
        return std::nullopt;
    }

    bool must_have_name = g_sections.at(std::string(section));

    std::string name;
    if (must_have_name) {
        int quotes_count = 0;
        for (char c : sanitized_line) {
            if (c == '"') {
                ++quotes_count;
            }
        }

        if (quotes_count != 2 || sanitized_line[sanitized_line.size() - 2] != '"') {
            log::err("Section \"{}\" does not have a name, line {}.", section, line_number);
            return std::nullopt;
        }

        std::vector<char> name_chars;
        bool seeking_name = false;
        for (char c : sanitized_line) {
            if (seeking_name && c != '"') {
                name_chars.push_back(c);
            }

            if (c == '"') {
                if (seeking_name) {
                    break;
                }

                seeking_name = true;
            }
        }

        name_chars.push_back('\0');

        name = std::string(name_chars.data());
    }

    return Section{std::string(section), name};
}

static bool read_int(const SectionBody& section_body, const std::string& key, int& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto& val = section_body.at(key).first;

    auto [ptr, ec] = std::from_chars(val.data(), val.data() + val.size(), dest);

    if (ec != std::errc()) {
        return false;
    }

    return true;
}

static void read_environment(Context& ctx, const SectionBody& environment_config)
{

}

static void read_sensor(Context& ctx, const SectionBody& sensor_config)
{

}

static void read_switch(Context& ctx, const Section& section, const SectionBody& switch_config)
{
    int gpio = 0;
    if (!read_int(switch_config, "gpio", gpio)) {
        log::err("Switch \"{}\" does not have valid GPIO pin specified.", section.name);
    }

    int oscillation_step = -1;
    if (switch_config.count("oscillation_step") != 0 && !read_int(switch_config, "oscillation_step", oscillation_step)) {
        log::err("Switch \"{}\" does not have valid oscillation_step.", section.name);
    }

    ctx.m_switches.push_back(Switch{section.name, gpio, oscillation_step});
}

static void read_timer(Context& ctx, const SectionBody& timer_config)
{

}

void parse_config(Context& ctx, std::string_view str)
{
    line_number = 0;

    ctx.m_switches.clear();

    ConfigMap conf;

    // Parse
    const auto lines = split(str, "\n");
    std::optional<Section> current_section = std::nullopt;

    for (int i = 0; i < lines.size(); ++i)
    {
        ++line_number;

        const auto sanitized_line = sanitize_line(lines[i]);
        if (sanitized_line.empty() || sanitized_line[0] == '#') {
            continue;
        }

        if (const auto maybe_section = get_section(sanitized_line)) {
            current_section = maybe_section;
        } else {
            if (current_section.has_value()) {
                // we are in a section, push lines
                auto& section_lines = conf[current_section.value()];

                // expecting key, value pair
                const auto key_val_pair = split(sanitized_line, "=");
                if (key_val_pair.size() != 2) {
                    log::err("Syntax error at line {}.", line_number);
                    continue;
                }

                section_lines[std::string(key_val_pair[0])] = {
                    std::string(key_val_pair[1]), line_number
                };
            }
        }
    }

    // Process config
    for (const auto& [section_header, section_body] : conf) {
        if (section_header.type == "environment") {
            // read_environment(ctx, conf);
        } else if (section_header.type == "sensor") {
        } else if (section_header.type == "switch") {
            read_switch(ctx, section_header, section_body);
        } else if (section_header.type == "timer") {
        }
    }
}
}
