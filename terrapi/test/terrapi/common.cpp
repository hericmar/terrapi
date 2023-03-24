#include "common.h"

terra::Config create_test_config()
{
    using namespace terra;

    Config config{};

    std::vector<SensorConfig> sensors = {
        SensorConfig{"dht11", 1, "Dummy"},
        SensorConfig{"water", 2, "Dummy"},
    };
    config.sensors = sensors;

    std::vector<SwitchConfig> switches = {
        SwitchConfig{
            "humidifier",
            1,
            "(dht11.humidity < 60) & (dht11.temperature < 25) & (water.signal = 1)",
            1000 / 1000,
            2000 / 1000
        },
        // SwitchConfig{"lights", 1, "09:00 < time & time < 20:12"}
    };
    config.switches = switches;

    return config;
}