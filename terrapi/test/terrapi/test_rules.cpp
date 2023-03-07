#include <string>

#include "doctest/doctest.h"

#include "context.h"
#include "datetime.h"
#include "expr.h"
#include "switch.h"

using namespace terra;

TEST_CASE("Can parse given expression")
{
    Config config{};

    std::vector<SensorConfig> sensors = {
        SensorConfig{"dht11", 1, "Dummy"},
    };
    config.sensors = sensors;

    Context::create(config);
    {
        std::string str = "dht11.temperature < 22";
        REQUIRE(Expr::from(str));
    }
    {
        std::string str = "(10:59 < time) & (time < 21:57)";
        REQUIRE(Expr::from(str));
    }
}

TEST_CASE("Can update given expression")
{
    Config config{};

    std::vector<SensorConfig> sensors = {
        SensorConfig{"dht11", 1, "Dummy"},
        SensorConfig{"water", 2, "Dummy"},
    };
    config.sensors = sensors;

    std::vector<SwitchConfig> switches = {
        SwitchConfig{"humidifier", 1, "(dht11.humidity < 60) & (dht11.temperature < 25) & (water.signal = 1)"},
        SwitchConfig{"lights", 1, "09:00 < time & time < 20:12"}
    };
    config.switches = switches;

    Context::create(config);

    auto* sensor_dht11 = (DummySensor*) ctx().get_sensor("dht11");
    REQUIRE(sensor_dht11);

    auto* sensor_water = (DummySensor*) ctx().get_sensor("water");
    REQUIRE(sensor_water);

    auto* humidifier = ctx().get_switch("humidifier");

    {
        // only one rule is valid

        sensor_dht11->force_value(ValueType_Humidity, 50.0f);
        sensor_dht11->force_value(ValueType_Temperature, 30.0f);

        ctx().tick();

        REQUIRE(!humidifier->is_on());
    }
    {
        // only one rule is valid

        sensor_dht11->force_value(ValueType_Humidity, 90.0f);
        sensor_dht11->force_value(ValueType_Temperature, 20.0f);

        ctx().tick();

        REQUIRE(!humidifier->is_on());
    }
    {
        // both rules are valid

        sensor_dht11->force_value(ValueType_Humidity, 50.0f);
        sensor_dht11->force_value(ValueType_Temperature, 20.0f);
        sensor_water->force_value(ValueType_Signal, 1.0f);

        ctx().tick();

        REQUIRE(humidifier->is_on());
    }
    {
        auto* lights = ctx().get_switch("lights");

        {
            // inactive time

            auto time = parse_time_from_str("04:00").value();
            ctx().clock()->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(!lights->is_on());
        }
        {
            // active time

            auto time = parse_time_from_str("14:00").value();
            ctx().clock()->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(lights->is_on());
        }
        {
            // inactive time

            auto time = parse_time_from_str("23:30").value();
            ctx().clock()->force_value(time);

            // ctx().tick();
            lights->update(time);

            REQUIRE(!lights->is_on());
        }
    }
}
