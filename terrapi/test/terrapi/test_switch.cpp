#include <chrono>
#include <thread>

#include "doctest/doctest.h"

#include "context.h"
#include "switch.h"

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
    curr_ctx().tick();
    auto t2 = now_ms();

    return t2 - t1;
}

TEST_CASE("Test switch oscillation")
{
    using namespace std::chrono_literals;

    Context::create(create_test_config());

    auto* sensor_dht11 = (DummySensor*) curr_ctx().get_sensor("dht11");
    REQUIRE(sensor_dht11);

    auto* sensor_water = (DummySensor*) curr_ctx().get_sensor("water");
    REQUIRE(sensor_water);

    // make humidifier switch active
    sensor_dht11->force_value(ValueType_Humidity, 50.0f);
    sensor_dht11->force_value(ValueType_Temperature, 20.0f);
    sensor_water->force_value(ValueType_Signal, 1.0f);

    auto* humidifier = curr_ctx().get_switch("humidifier");

    // at create_test_config
    auto high_ms = 1000;
    auto low_ms = 2000;

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
        std::this_thread::sleep_for(2500ms);

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(!humidifier->is_on_high());

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(!humidifier->is_on_high());
    }
    {
        std::this_thread::sleep_for(1500ms);

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());

        tick();
        REQUIRE(humidifier->is_on());
        REQUIRE(humidifier->is_on_high());
    }
}
