#include <atomic>
#include <iostream>
#include <math.h>
#include <thread>

#include "core/core.h"
#include "common.h"

using namespace terra;

int main()
{
    auto config = create_test_config();

    auto* ctx = Core::create(config);

    auto* sensor_dht11 = (DummySensor*) core().context().get_sensor("dht11");
    sensor_dht11->force_value(HUMIDITY, 50.0f);
    sensor_dht11->force_value(TEMPERATURE, 20.0f);

    auto* sensor_water = (DummySensor*) core().context().get_sensor("water");
    sensor_water->force_value(SIGNAL, 1.0f);

    std::atomic<bool> running = true;
    const auto thread = std::thread([&]() {
        while (running) {
            const auto now = std::chrono::system_clock::now();
            const auto now_seconds = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch();

            const auto rads = now_seconds.count() * (M_PI / 180);
            const auto value = (float) sin(rads);

            sensor_dht11->force_value(HUMIDITY, value);
            std::cout << "set humidity to " << value << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    });

    ctx->run();
}
