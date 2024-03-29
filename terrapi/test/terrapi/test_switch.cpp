// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include <chrono>
#include <thread>

#include "doctest/doctest.h"

#include "core/core.h"
#include "core/switch.h"

#include "common.h"

using namespace terra;

std::size_t now_ms()
{
    auto time_start = std::chrono::system_clock::now();
    auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();

    return time_start_ms;
}

std::size_t tick()
{
    auto t1 = now_ms();
    core().tick();
    auto t2 = now_ms();

    return t2 - t1;
}

TEST_CASE("Test switch oscillation")
{
    using namespace std::chrono_literals;

    Core::create(create_test_config());

    auto* sensor_dht11 = (DummySensor*) core().context().get_sensor("dht11");
    REQUIRE(sensor_dht11);

    auto* sensor_water = (DummySensor*) core().context().get_sensor("water");
    REQUIRE(sensor_water);

    // make humidifier switch active
    sensor_dht11->force_value(HUMIDITY, 50.0f);
    sensor_dht11->force_value(TEMPERATURE, 20.0f);
    sensor_water->force_value(SIGNAL, 1.0f);

    auto* humidifier = core().context().get_switch("humidifier");

    // at create_test_config
    auto high_ms = 1000;
    auto low_ms = 2000;

    /// \todo fix this test
    /*
    {
        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());
    }
    {
        // ???
        std::this_thread::sleep_for(1000ms);

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(!humidifier->is_on_high());

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(!humidifier->is_on_high());
    }
    {
        std::this_thread::sleep_for(1000ms);

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());
    }
     */
}
