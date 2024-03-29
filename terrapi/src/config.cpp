// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <set>
#include "config.h"

#include "toml.hpp"

#include "hardware/config.h"
#include "logger.h"
#include "utils.h"

#define RETURN_EMPTY(message) LOG(FATAL, message); return std::nullopt

namespace terra
{
/// @returns missing config key
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

    if (table.contains("day_from")) {
        if (auto maybe_value = parse_time_from_str(table["day_from"].ref<std::string>())) {
            config.day_from = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for day_from");
        }
    }

    if (table.contains("day_to")) {
        if (auto maybe_value = parse_time_from_str(table["day_to"].ref<std::string>())) {
            config.day_to = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for day_to");
        }
    }

    if (table.contains("measure_step")) {
        if (auto maybe_value = table["measure_step"].value<int>()) {
            config.measure_step = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for measure_step");
        }
    }

    if (table.contains("publish_step")) {
        if (auto maybe_value = table["publish_step"].value<int>()) {
            config.publish_step = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for publish_step");
        }
    }

    return config;
}

std::optional<BrokerConfig> parse_broker_config(const toml::table& table)
{
    BrokerConfig config{};

    if (table.contains("enabled") && table["enabled"].is<bool>()) {
        config.enabled = *table["enabled"].value<bool>();
    }

    if (!config.enabled) {
        // broker is disabled, no need to parse further
        return config;
    }

    if (auto missing_key = missing_values(table, { "address", "client_id", "password" })) {
        RETURN_EMPTY("missing config key " + missing_key.value());
    }

    /// @todo Validate inet address before use.
    config.address = table["address"].ref<std::string>();
    if (config.address.back() == '/') {
        config.address.pop_back();
    }

    config.client_id = table["client_id"].ref<std::string>();
    config.password = table["password"].ref<std::string>();

    return config;
}

std::optional<SensorConfig> parse_sensor_config(std::string name, const toml::table& table)
{
    SensorConfig config{};

    config.name = name;

    if (auto missing_key = missing_values(table, { "type" })) {
        RETURN_EMPTY("missing config key " + missing_key.value());
    }

    config.sensor_type = table["type"].ref<std::string>();
    if (KNOWN_GPIO_SENSORS.count(config.sensor_type) == 0 && KNOWN_I2C_SENSORS.count(config.sensor_type) == 0) {
        RETURN_EMPTY("unknown sensor type " + config.sensor_type);
    }

    if (KNOWN_GPIO_SENSORS.count(config.sensor_type)) {
        if (auto maybe_value = table["gpio"].value<int>()) {
            config.gpio = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for gpio");
        }
    }

    return config;
}

std::optional<DeviceConfig> parse_device_config(std::string name, const toml::table& table)
{
    DeviceConfig config{};

    config.name = name;

    if (auto missing_key = missing_values(table, { "gpio", "rule" })) {
        RETURN_EMPTY("missing config key " + missing_key.value());
    }

    if (auto maybe_value = table["gpio"].value<int>()) {
        config.gpio = *maybe_value;
    } else {
        RETURN_EMPTY("invalid value for gpio");
    }

    config.rule = table["rule"].ref<std::string>();
    // trim spaces
    config.rule.erase(std::remove_if(config.rule.begin(), config.rule.end(), isspace), config.rule.end());

    if (table.contains("power")) {
        if (auto maybe_value = table["power"].value<float>()) {
            config.power = *maybe_value;
        } else {
            RETURN_EMPTY("invalid value for power");
        }
    }

    if (table.contains("oscillation_high") && table.contains("oscillation_low")) {
        auto maybe_high = table["oscillation_high"].value<int>();
        auto maybe_low = table["oscillation_low"].value<int>();

        if (!maybe_high.has_value() || !maybe_low.has_value()) {
            config.oscillation_high_ms = *maybe_high;
            config.oscillation_low_ms = *maybe_low;
        }
    }

    return config;
}

std::optional<Config> parse_config(const toml::table& table)
{
    Config config{};

    if (auto missing_key = missing_values(table, { "environment" })) {
        RETURN_EMPTY("missing config key " + *missing_key);
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

    std::set<std::string> sensor_names;
    if (const toml::table* sensors = table["sensor"].as_table()) {
        for (const auto& [key, value] : *sensors) {
            std::string name(key.begin(), key.end());
            if (auto maybe_sensor = parse_sensor_config(name, *value.as_table())) {
                config.sensors.push_back(*maybe_sensor);
                sensor_names.insert(name);
            } else {
                LOG(ERR, "unable to process sensor config");
            }
        }
    } else {
        LOG(WARN, "no sensors specified");
    }

    if (const toml::table* devices = table["device"].as_table()) {
        for (const auto& [key, value] : *devices) {
            std::string name(key.begin(), key.end());
            if (sensor_names.count(name)) {
                LOG(ERR, "device name " + name + " is already used by a sensor");
                continue;
            }

            if (auto maybe_device = parse_device_config(name, *value.as_table())) {
                config.devices.push_back(*maybe_device);
            } else {
                LOG(ERR, "unable to process device config");
            }
        }
    } else {
        LOG(WARN, "no devices specified");
    }

    return config;
}

std::optional<Config> Config::from_file(const char* path)
{
    std::ifstream t(path);
    if (!t.good()) {
        RETURN_EMPTY("cannot open file " + std::string(path));
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
        RETURN_EMPTY("config parsing failed: " + stream.str());
    }
}
}
