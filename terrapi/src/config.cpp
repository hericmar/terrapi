#include <array>
#include <charconv>
#include <optional>
#include <map>

#include "terrapi/terrapi.h"

#include "config.h"
#include "chrono.h"

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

//------------------------------------------------------------------------------

static int line_number = 0;
static std::map<std::string, int> sensor_name_idx_map;

//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------

static bool read_float(const std::string& val, float& dest)
{
    try {
        dest = std::stof(val);

        return true;
    } catch (...) {
        return false;
    }
}

static bool read_int(const std::string& val, int& dest)
{
    auto [ptr, ec] = std::from_chars(val.data(), val.data() + val.size(), dest);

    if (ec != std::errc()) {
        return false;
    }

    return true;
}

static bool read_int(const SectionBody& section_body, const std::string& key, int& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto& val = section_body.at(key).first;

    return read_int(val, dest);
}

static bool read_time(const SectionBody& section_body, const std::string& key, int& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto& val = section_body.at(key).first;

    return str_to_time(val, dest);
}

static bool read_time_interval(const SectionBody& section_body, const std::string& key, ValueInterval& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto values = split(section_body.at(key).first, ",");

    if (values.size() != 2) {
        return false;
    }

    auto result = str_to_time(std::string(values[0]), dest[0].int_val);

    return result && str_to_time(std::string(values[1]), dest[1].int_val);
}

static bool read_float_interval(const SectionBody& section_body, const std::string& key, ValueInterval& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto values = split(section_body.at(key).first, ",");

    if (values.size() != 2) {
        return false;
    }

    auto result = read_float(std::string(values[0]), dest[0].float_val);

    return result && read_float(std::string(values[1]), dest[1].float_val);
}

//------------------------------------------------------------------------------

static bool read_environment(Context& ctx, const SectionBody& environment_config)
{
    ValueInterval daytime{};

    if (!read_time(environment_config, "day_from", daytime[0].int_val)) {
        log::err("Switch \"environment\" does not have valid GPIO pin specified.");
        return false;
    }

    if (!read_time(environment_config, "day_to", daytime[1].int_val)) {
        log::err("Switch \"environment\" does not have valid GPIO pin specified.");
        return false;
    }

    ctx.m_daytime[0] = daytime[0].int_val;
    ctx.m_daytime[1] = daytime[1].int_val;

    return true;
}

static bool read_sensor(Context& ctx, const Section& section, const SectionBody& sensor_config)
{
    std::optional<ValueInterval> day_interval{ValueInterval()};
    std::optional<ValueInterval> night_interval{ValueInterval()};

    EPhysicalQuantity q;
    if (sensor_config.count("type") == 0) {
        log::err(R"(Sensor "{}" does not have valid "type" specified.)", section.name);
        return false;
    }

    if (sensor_config.count("active_interval") == 0) {
        log::err(R"(Sensor "{}" does not have valid "active_interval" specified.)", section.name);
        return false;
    }

    if (sensor_config.at("type").first == "DHT11") {

        if (sensor_config.count("physical_quantity") == 0) {
            log::err(R"(Sensor "{}" does not have valid "physical_quantity" specified.)", section.name);
            return false;
        }

        if (auto maybe_q = from_string(sensor_config.at("physical_quantity").first)) {
            q = *maybe_q;
        } else {
            log::err(R"(Sensor "{}" does not have valid "physical_quantity" specified.)", section.name);
            return false;
        }

        int gpio = 0;
        if (!read_int(sensor_config, "gpio", gpio)) {
            log::err(R"(Sensor "{}" does not have valid "gpio" specified.)", section.name);
            return false;
        }

        if (!read_float_interval(sensor_config, "active_interval", *day_interval)) {
            log::err(R"(Sensor "{}" does not have valid "active_interval".)", section.name);
            return false;
        }

        if (sensor_config.count("night_active_interval")) {
            if (sensor_config.at("night_active_interval").first == "none") {
                night_interval = std::nullopt;
            } else {
                if (!read_float_interval(sensor_config, "night_active_interval", *night_interval)) {
                    log::err(R"(Sensor "{}" does not have valid "night_active_interval".)", section.name);
                    return false;
                }
            }
        }

        ctx.m_sensors.push_back(std::make_unique<DHT11>(gpio));
        ctx.m_controllers.push_back(SensorController(
            section.name,
            ctx.m_sensors[ctx.m_sensors.size() - 1].get(),
            q,
            day_interval,
            night_interval
        ));

    } else {
        log::err(R"(Sensor "{}" has unknown "type" specified.)", section.name);
        return false;
    }

    sensor_name_idx_map[section.name] = ctx.m_controllers.size() - 1;

    return true;
}

static bool read_switch(Context& ctx, const Section& section, const SectionBody& switch_config)
{
    if (switch_config.count("sensor") == 0) {
        log::err(R"(Switch "{}" does not have "sensor" specified.)", section.name);
        return false;
    }

    const auto& sensor_name = switch_config.at("sensor").first;
    if (sensor_name_idx_map.find(sensor_name) == sensor_name_idx_map.end()) {
        log::err(R"(Switch "{}" has invalid "sensor" specified, sensor "{}" does not exist.)", section.name, sensor_name);
        return false;
    }

    int gpio = 0;
    if (!read_int(switch_config, "gpio", gpio)) {
        log::err("Switch \"{}\" does not have valid GPIO pin specified.", section.name);
        return false;
    }

    int oscillation_step = -1;
    if (switch_config.count("oscillation_step") && !read_int(switch_config, "oscillation_step", oscillation_step)) {
        log::err("Switch \"{}\" does not have valid oscillation_step.", section.name);
        return false;
    }

    ctx.m_switches.push_back(Switch{section.name, gpio, oscillation_step});

    auto& controller = ctx.m_controllers[sensor_name_idx_map[sensor_name]];
    controller.set_switch_id(ctx.m_switches.size() - 1);

    return true;
}

static bool read_timer(Context& ctx, const Section& section, const SectionBody& timer_config)
{
    ValueInterval interval;
    if (!read_time_interval(timer_config, "active_interval", interval)) {
        log::err(R"(Cannot read "active_interval" for timer "{}".)", section.name);
        return false;
    }

    ctx.m_controllers.push_back(SensorController(section.name, ctx.m_clock.get(), EPhysicalQuantity::Time, interval));
    sensor_name_idx_map[section.name] = ctx.m_controllers.size() - 1;

    return true;
}

//------------------------------------------------------------------------------

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
            read_environment(ctx, section_body);
        } else if (section_header.type == "sensor") {
            read_sensor(ctx, section_header, section_body);
        } else if (section_header.type == "timer") {
            read_timer(ctx, section_header, section_body);
        }
    }

    for (const auto& [section_header, section_body] : conf) {
        if (section_header.type == "switch") {
            read_switch(ctx, section_header, section_body);
        }
    }
}
}
