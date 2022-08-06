#include <array>
#include <charconv>
#include <optional>
#include <map>
#include <set>

#include "terrapi/terrapi.h"

#include "config.h"
#include "chrono.h"
#include "rule.h"

namespace terra
{
struct SectionComparator
{
    bool operator()(const Section& lhs, const Section& rhs) const {
        return lhs.type < rhs.type || (!(rhs.type < lhs.type) && lhs.name < rhs.name);
    }
};

using ConfigMap = std::map<
    Section,
    SectionBody,
    SectionComparator
>;

/// {section, has_name}
static const std::map<std::string, bool> g_sections = {
    {"general", false},
    {"environment", false},
    {"sensor", true},
    {"switch", true},
    {"timer", true}
};

//------------------------------------------------------------------------------

static int line_number = 0;

//------------------------------------------------------------------------------

bool is_str_literal(int c)
{
    return std::isalnum(c) || c == '.';
}

bool is_time_char(int c)
{
    return std::isalnum(c) || c == ':';
}

//------------------------------------------------------------------------------

static std::string sanitize_line(std::string_view line)
{
    std::vector<char> sanitized;
    sanitized.reserve(line.size());

    for (char c : line) {
        if (string_utils::is_whitespace(c)) {
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

        if (std::find(reserved_keywords.begin(), reserved_keywords.end(), name) != reserved_keywords.end()) {
            log::err("Your sensor/switch cannot have reserved name \"{}\", line {}.", name, line_number);
            return std::nullopt;
        }
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

static bool read_time(const SectionBody& section_body, const std::string& key, float& dest)
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

    const auto values = string_utils::split(section_body.at(key).first, ",");

    if (values.size() != 2) {
        return false;
    }

    auto result = str_to_time(std::string(values[0]), dest[0]);

    return result && str_to_time(std::string(values[1]), dest[1]);
}

static bool read_float_interval(const SectionBody& section_body, const std::string& key, ValueInterval& dest)
{
    if (section_body.count(key) == 0) {
        return false;
    }

    const auto values = string_utils::split(section_body.at(key).first, ",");

    if (values.size() != 2) {
        return false;
    }

    auto result = read_float(std::string(values[0]), dest[0]);

    return result && read_float(std::string(values[1]), dest[1]);
}

//------------------------------------------------------------------------------

static bool read_general(Context& ctx, const SectionBody& general_config)
{
    if (general_config.count("broker_address") == 0 ||
        general_config.count("client_id") == 0 ||
        general_config.count("password") == 0) {

        log::info(R"(No "broker_address" or "client_id" or "password" specified, running in the single device mode.)");
        return true;
    }

    ctx.m_broker_addr = general_config.at("broker_address").first;
    ctx.m_client_id   = general_config.at("client_id").first;
    ctx.m_password    = general_config.at("password").first;

    return true;
}

static bool read_environment(Context& ctx, const SectionBody& environment_config)
{
    ValueInterval daytime{};

    if (!read_time(environment_config, "day_from", daytime[0])) {
        log::err("Switch \"environment\" does not have valid GPIO pin specified.");
        return false;
    }

    if (!read_time(environment_config, "day_to", daytime[1])) {
        log::err("Switch \"environment\" does not have valid GPIO pin specified.");
        return false;
    }

    if (read_int(environment_config, "measure_step", ctx.m_measure_step)) {
        ctx.m_measure_step *= 1000;
    }

    if (read_int(environment_config, "data_post_step", ctx.m_data_post_step)) {
        ctx.m_data_post_step *= 1000;
    }

    ctx.m_daytime[0] = daytime[0];
    ctx.m_daytime[1] = daytime[1];

    return true;
}

static bool read_sensor(Context& ctx, const Section& section, const SectionBody& sensor_config)
{
    static std::set<unsigned> used_gpios;

    if (sensor_config.count("type") == 0) {
        log::err(R"(Sensor "{}" does not have valid "type" specified.)", section.name);
        return false;
    }

    if (sensor_config.at("type").first == "DHT11") {

        int gpio = 0;
        if (!read_int(sensor_config, "gpio", gpio)) {
            log::err(R"(Sensor "{}" does not have valid "gpio" specified.)", section.name);
            return false;
        }

        if (used_gpios.count(gpio) == 0) {
            ctx.m_sensors.push_back(std::make_unique<DHT11>(section.name, gpio));
            used_gpios.insert(gpio);
        }

    } else {
        log::err(R"(Sensor "{}" has unknown "type" specified.)", section.name);
        return false;
    }

    return true;
}

bool read_switch(Context& ctx, const Section& section, const SectionBody& switch_config)
{
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

    Expr rules;

    if (switch_config.count("rule") == 0) {
        log::err(R"(Switch "{}" does not have "rule" specified.)", section.name);
        return false;
    } else {
        try {
            rules = create_expr_tree(switch_config.at("rule").first);
        } catch (const std::exception& e) {
            log::err(e.what());
            log::err("Switch \"{}\" does not have valid rules.", section.name);

            return false;
        }
    }

    auto s = Switch{section.name, gpio, rules, oscillation_step};

    ctx.m_switch_controllers.push_back(Controller{s});

    return true;
}

//------------------------------------------------------------------------------

bool parse_config(Context& ctx, std::string_view str)
{
    line_number = 0;

    ConfigMap conf;

    // Parse
    const auto lines = string_utils::split(str, "\n");
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
                const auto key_val_pair = string_utils::split(sanitized_line, "=");
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
    bool result = true;

    for (const auto& [section_header, section_body] : conf) {
        if (section_header.type == "general") {
            result = result && read_general(ctx, section_body);
        } else if (section_header.type == "environment") {
            result = result && read_environment(ctx, section_body);
        } else if (section_header.type == "sensor") {
            result = result && read_sensor(ctx, section_header, section_body);
        }
    }

    for (const auto& [section_header, section_body] : conf) {
        if (section_header.type == "switch") {
            result = result && read_switch(ctx, section_header, section_body);
        }
    }

    return result;
}
}
