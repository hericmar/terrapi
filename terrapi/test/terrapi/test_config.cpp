#include "tests.h"

#include <cassert>

#include "terrapi/terrapi.h"

//------------------------------------------------------------------------------

static constexpr const char* empty_config = R"(
)";

void test_empty_config()
{
    auto& app = terra::App::create_from_string(empty_config);
}

//------------------------------------------------------------------------------

static constexpr const char* basic_config = R"(
[environment]
day_from = 09:00
day_to = 17:00

# Sensor is one physical quantity representation of physical sensor.
# In this example we use sensor.id := 0 for measuring humidity,
# although the DHT11 can measure temperature and humidity.
[sensor "dht/temperature"]
id = 0
type = DHT11
wp_gpio = 7
physical_quantity = humidity
active_interval = 75.0, 100.0
night_active_interval = 60.0, 80.0

[sensor dht/humidity]
id = 1
type = DHT11
wp_gpio = 11
physical_quantity = temperature
active_interval = 22.0, 24.0
night_active_interval = none

[timer]
id = 1
active_interval = 10:59, 20:57

[switch "heater"]
wp_gpio = 0
sensor_id = 0

[switch humidifier]
wp_gpio = 2
sensor_id = 1
oscillation_step = 2000
)";

void test_basic_config()
{
    auto& app = terra::App::create_from_string(basic_config);

    assert(app.switches().size() == 2);
    assert(app.switches()[0].name() == "heater");
    assert(app.switches()[0].name() == "humidifier");
}

//------------------------------------------------------------------------------


void test_config()
{
    test_empty_config();
    test_basic_config();
}
