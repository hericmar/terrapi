#include "config.h"

#include "toml.hpp"

#include "logger.h"

#define RETURN_EMPTY(message) log_message("fatal", message); return std::nullopt

namespace terra
{
/// @returns missing key
std::optional<std::string> missing_values(const toml::table& table, const std::vector<const char*>& expected_keys)
{
    for (const char* key : expected_keys) {
        if (!table.contains(key)) {
            return key;
        }
    }

    return std::nullopt;
}

std::optional<EnvironmentConfig> parse_environment_config(const toml::table& table)
{
    EnvironmentConfig config{};

    if (auto missing_key = missing_values(table, { "day_from", "day_to", "measure_step", "publish_step" })) {
        RETURN_EMPTY("missing key " + missing_key.value());
    }

    if (auto maybe_value = parse_time_from_str(table["day_from"].ref<std::string>())) {
        config.day_from = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for day_from");
    }

    if (auto maybe_value = parse_time_from_str(table["day_to"].ref<std::string>())) {
        config.day_to = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for day_to");
    }

    if (auto maybe_value = table["measure_step"].value<int>()) {
        config.measure_step = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for measure_step");
    }

    if (auto maybe_value = table["publish_step"].value<int>()) {
        config.publish_step = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for publish_step");
    }

    return config;
}

std::optional<BrokerConfig> parse_broker_config(const toml::table& table)
{
    BrokerConfig config{};

    if (auto missing_key = missing_values(table, { "address", "client_id", "password" })) {
        RETURN_EMPTY("missing key " + missing_key.value());
    }

    /// @todo Validate inet address before use.
    config.address = table["address"].ref<std::string>();
    if (config.address.back() != '/') {
        config.address.push_back('/');
    }

    config.client_id = table["client_id"].ref<std::string>();
    config.password = table["password"].ref<std::string>();

    return config;
}

std::optional<SensorConfig> parse_sensor_config(std::string name, const toml::table& table)
{
    SensorConfig config{};

    config.name = name;

    if (auto missing_key = missing_values(table, { "type", "gpio" })) {
        RETURN_EMPTY("missing key " + missing_key.value());
    }

    config.sensor_type = table["type"].ref<std::string>();
    if (sensor_types.count(config.sensor_type) == 0) {
        RETURN_EMPTY("unknown sensor type " + config.sensor_type);
    }

    if (auto maybe_value = table["gpio"].value<int>()) {
        config.gpio = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for gpio");
    }

    return config;
}

std::optional<SwitchConfig> parse_switch_config(std::string name, const toml::table& table)
{
    SwitchConfig config{};

    config.name = name;

    if (auto missing_key = missing_values(table, { "gpio", "rule" })) {
        RETURN_EMPTY("missing key " + missing_key.value());
    }

    if (auto maybe_value = table["gpio"].value<int>()) {
        config.gpio = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for gpio");
    }

    config.rule = table["rule"].ref<std::string>();

    return config;
}

std::optional<Config> parse_config(const toml::table& table)
{
    Config config{};

    if (auto missing_key = missing_values(table, { "environment" })) {
        RETURN_EMPTY("missing key " + *missing_key);
    }

    if (const toml::table* environment = table["environment"].as_table()) {
        if (auto maybe_environment_config = parse_environment_config(*environment)) {
            config.environment = maybe_environment_config.value();
        } else {
            RETURN_EMPTY("unable to process environment config");
        }
    }

    // optional
    if (const toml::table* broker = table["broker"].as_table()) {
        if (auto maybe_broker_config = parse_broker_config(*broker)) {
            config.broker = maybe_broker_config.value();
        } else {
            RETURN_EMPTY("unable to process broker config");
        }
    }

    if (const toml::table* sensors = table["sensor"].as_table()) {
        for (const auto& [key, value] : *sensors) {
            std::string name(key.begin(), key.end());
            if (auto maybe_sensor = parse_sensor_config(name, *value.as_table())) {
                config.sensors.push_back(*maybe_sensor);
            } else {
                log_message("error", "unable to process sensor config");
            }
        }
    } else {
        log_message("warn", "no sensors specified");
    }

    if (const toml::table* switches = table["switch"].as_table()) {
        for (const auto& [key, value] : *switches) {
            std::string name(key.begin(), key.end());
            if (auto maybe_switch = parse_switch_config(name, *value.as_table())) {
                config.switches.push_back(*maybe_switch);
            } else {
                log_message("error", "unable to process switch config");
            }
        }
    } else {
        log_message("warn", "no switches specified");
    }

    return config;
}

std::optional<Config> Config::from_file(const char* path)
{
    std::ifstream t(path);
    if (!t.good()) {
        log_message("fatal", "cannot open file " + std::string(path));
        return std::nullopt;
    }

    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

    return from_str(str.c_str());
}

std::optional<Config> Config::from_str(const char* str)
{
    try {
        toml::table table = toml::parse(str);

        if (auto maybe_config = parse_config(table)) {
            auto config = *maybe_config;
            config.raw = str;

            return config;
        } else {
            return std::nullopt;
        }
    }
    catch (const toml::parse_error& err) {
        std::stringstream stream;
        stream << err;
        log_message("fatal", "config parsing failed: " + stream.str());

        return std::nullopt;
    }
}
}
