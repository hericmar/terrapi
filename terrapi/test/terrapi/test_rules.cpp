// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <string>

#include "doctest/doctest.h"

#include "core/core.h"
#include "core/expr.h"
#include "utils.h"

#include "common.h"

using namespace terra;

TEST_CASE("Parsing given expression")
{
    struct TestParams
    {
        std::string expr;
        bool should_pass;
    };

    Config config{};

    std::vector sensors = {
        SensorConfig{"dht11", 1, "DHT11"},
    };
    config.sensors = sensors;

    Core::create(config);

    const std::vector<TestParams> tests = {
        {"dht11.temperature = 22", true},
        {"dht11.temperature < 22", true},
        {"dht11.temperature < 22.", true},
        {"dht11.temperature < 22.10", true},
        // Unknown sensor
        {"unknown.temperature < 22", false},
        // Unknown physical quantity
        {"dht11.force < 22", false},
        {"(10:59 < time) & (time < 21:57)", true},
        {"((", false},
        {"((())", false},
        {"(10: < time)", false},
        // Technically valid, HH:MM time is converted to seconds since midnight.
        {"(10 < time)", true},
    };

    for (const auto& test : tests) {
        auto result = Expr::from(core().context().sensors, test.expr);
        REQUIRE_EQ(test.should_pass, result.has_value());
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

        sensor_dht11->force_value(HUMIDITY, 50.0f);
        sensor_dht11->force_value(TEMPERATURE, 30.0f);

        humidifier->update(0);

        REQUIRE(!humidifier->is_on());
    }
    {
        // only one rule is valid

        sensor_dht11->force_value(HUMIDITY, 90.0f);
        sensor_dht11->force_value(TEMPERATURE, 20.0f);

        humidifier->update(0);

        REQUIRE(!humidifier->is_on());
    }
    {
        // both rules are valid

        sensor_dht11->force_value(HUMIDITY, 50.0f);
        sensor_dht11->force_value(TEMPERATURE, 20.0f);
        sensor_water->force_value(SIGNAL, 1.0f);

        humidifier->update(0);

        REQUIRE(humidifier->is_on());
    }
    {
        auto* lights = core().context().get_switch("lights");

        auto t        = std::time(nullptr);
        std::tm tm    = *std::gmtime(&t);
        tm.tm_hour    = 0;
        tm.tm_min     = 0;
        tm.tm_sec     = 0;
        auto midnight = (int64_t) std::mktime(&tm);

        {
            // inactive time

            const auto daytime = parse_time_from_str("04:00").value();
            auto time          = midnight + daytime;
            core().context().clock->force_value(time);

            // ctx().tick();
            lights->update(time * 1000);

            REQUIRE(!lights->is_on());
        }
        {
            // active time

            const auto daytime = parse_time_from_str("14:00").value();
            auto time          = midnight + daytime;
            core().context().clock->force_value(daytime);

            // ctx().tick();
            lights->update(time * 1000);

            REQUIRE(lights->is_on());
        }
        {
            // inactive time

            const auto daytime = parse_time_from_str("23:30").value();
            auto time          = midnight + daytime;
            core().context().clock->force_value(daytime);

            // ctx().tick();
            lights->update(time * 1000);

            REQUIRE(!lights->is_on());
        }
    }
}
