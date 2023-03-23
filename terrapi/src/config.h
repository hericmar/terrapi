#pragma once

#include <optional>
#include <string>
#include <vector>

#include "datetime.h"
#include "expr.h"

namespace terra
{
struct EnvironmentConfig
{
    Time     day_from;
    Time     day_to;
    /// in milliseconds
    unsigned measure_step;
    /// in milliseconds
    unsigned publish_step;
};

struct BrokerConfig
{
    std::string address;
    std::string client_id;
    std::string password;
};

struct SensorConfig
{
    std::string name;
    int         gpio;
    std::string sensor_type;
};

struct SwitchConfig
{
    std::string name;
    int         gpio;
    std::string rule;
    /// in seconds, not set by default
    int         oscillation_high = -1;
    /// in seconds, not set by default
    int         oscillation_low = -1;
};

struct Config
{
    EnvironmentConfig         environment;
    BrokerConfig              broker;
    std::vector<SensorConfig> sensors;
    std::vector<SwitchConfig> switches;

    std::string raw;

    static std::optional<Config> from_file(const char* path);
    static std::optional<Config> from_str(const char* str);
};
}
