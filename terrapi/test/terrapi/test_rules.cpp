#include <string>

#include "doctest/doctest.h"

#include "core/core.h"
#include "datetime.h"
#include "expr.h"
#include "switch.h"

#include "common.h"

using namespace terra;

TEST_CASE("Can parse given expression")
{
    Config config{};

    std::vector<SensorConfig> sensors = {
        SensorConfig{"dht11", 1, "Dummy"},
    };
    config.sensors = sensors;

    Core::create(config);
    {
        std::string str = "dht11.temperature < 22";
        REQUIRE(Expr::from(core().context().sensors, str));
    }
    {
        std::string str = "(10:59 < time) & (time < 21:57)";
        REQUIRE(Expr::from(core().context().sensors, str));
    }
}

TEST_CASE("Can update given expression")
{
    Core::create(create_test_config());

    auto* sensor_dht11 = (DummySensor*) core().context().get_sensor("dht11");
    REQUIRE(sensor_dht11);

    auto* sensor_water = (DummySensor*) core().context().get_sensor("water");
    REQUIRE(sensor_water);

    auto* humidifier = core().context().get_switch("humidifier");

    {
        // only one rule is valid

        sensor_dht11->force_value(ValueType_Humidity, 50.0f);
        sensor_dht11->force_value(ValueType_Temperature, 30.0f);

        humidifier->update(0);

        REQUIRE(!humidifier->is_on());
    }
    {
        // only one rule is valid

        sensor_dht11->force_value(ValueType_Humidity, 90.0f);
        sensor_dht11->force_value(ValueType_Temperature, 20.0f);

        humidifier->update(0);

        REQUIRE(!humidifier->is_on());
    }
    {
        // both rules are valid

        sensor_dht11->force_value(ValueType_Humidity, 50.0f);
        sensor_dht11->force_value(ValueType_Temperature, 20.0f);
        sensor_water->force_value(ValueType_Signal, 1.0f);

        humidifier->update(0);

        REQUIRE(humidifier->is_on());
    }
    {
        auto* lights = core().context().get_switch("lights");

        {
            // inactive time

            auto time = parse_time_from_str("04:00").value();
            core().context().clock->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(!lights->is_on());
        }
        {
            // active time

            auto time = parse_time_from_str("14:00").value();
            core().context().clock->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(lights->is_on());
        }
        {
            // inactive time

            auto time = parse_time_from_str("23:30").value();
            core().context().clock->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(!lights->is_on());
        }
    }
}
