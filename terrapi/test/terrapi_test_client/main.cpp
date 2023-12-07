#include "core/core.h"
#include "common.h"

using namespace terra;

int main()
{
    auto config = create_test_config();

    config.switches.push_back(SwitchConfig{"oscillator", 1, "1", 1000, 2000});

    auto* ctx = Core::create(config);

    auto* sensor_dht11 = (DummySensor*) core().context().get_sensor("dht11");
    sensor_dht11->force_value(HUMIDITY, 50.0f);
    sensor_dht11->force_value(TEMPERATURE, 20.0f);

    auto* sensor_water = (DummySensor*) core().context().get_sensor("water");
    sensor_water->force_value(SIGNAL, 1.0f);

    ctx->run();
}
