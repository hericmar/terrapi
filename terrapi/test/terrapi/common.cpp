#include <cassert>
#include "common.h"

constexpr const char* CONFIG_TOML = R"(
[environment]
measure_step = 1000
publish_step = 5000

[broker]
enabled = true
address = "http://localhost:8091"
client_id = "BuLOGvzA"
password = "dKm7LHwdov7WO5gjRDwhYZslaAeRSniO"

[sensor."dht11"]
gpio = 1
type = "DHT11"

[sensor."water"]
gpio = 2
type = "Signal"

[device."humidifier"]
gpio = 1
rule = " ((dht11.humidity < 60) & (dht11.temperature < 25)) & (water.signal = 1) "
power = 20.1
oscillation_high_ms = 1000
oscillation_low_ms = 2000

[device."lights"]
gpio = 1
power = 50
rule = "(09:00 < time) & (time < 20:12) "
)";

terra::Config create_test_config()
{
    using namespace terra;

    auto maybe_config = Config::from_str(CONFIG_TOML);
    assert(maybe_config.has_value());

    return *maybe_config;
}
