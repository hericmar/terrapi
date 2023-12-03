#include "context.h"
#include "common.h"

using namespace terra;

int main()
{
    const auto config = create_test_config();

    auto* ctx = Context::create(config);

    auto* sensor_dht11 = (DummySensor*) curr_ctx().get_sensor("dht11");
    sensor_dht11->force_value(ValueType_Humidity, 50.0f);
    sensor_dht11->force_value(ValueType_Temperature, 20.0f);

    auto* sensor_water = (DummySensor*) curr_ctx().get_sensor("water");
    sensor_water->force_value(ValueType_Signal, 1.0f);

    ctx->run();
}
