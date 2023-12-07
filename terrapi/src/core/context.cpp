#include "context.h"

#include <thread>

#include "core/core.h"
#include "sensor/factory.h"
#include "logger.h"
#include "sensor.h"
#include "switch.h"

namespace terra
{
std::unique_ptr<Context> Context::create(const Config& config)
{
    auto ctx = std::make_unique<Context>();

    ctx->sensors["time"] = std::make_unique<Clock>();
    ctx->clock = (Clock*) ctx->sensors["time"].get();

    for (const auto &sensor_config: config.sensors) {
        ctx->sensors.insert({
            sensor_config.name,
            sensor_factory.at(sensor_config.sensor_type)(sensor_config.gpio)
        });
    }

    for (const auto& switch_config : config.switches) {
        const auto maybe_expr = Expr::from(ctx->sensors, switch_config.rule);
        if (!maybe_expr) {
            log_message(ERR, "invalid rule for switch " + switch_config.name);
            continue;
        }

        ctx->switches.insert({
            switch_config.name,
            Switch((SwitchConfig*) &switch_config, *maybe_expr)
        });
    }

    return ctx;
}

Sensor* Context::get_sensor(const std::string& name) const
{
    assert(sensors.count(name));

    return sensors.at(name).get();
}

Switch* Context::get_switch(const std::string& name) const
{
    assert(switches.count(name));

    return (Switch*) &switches.at(name);
}

/*
void Context::post_records()
{
    HttpClient client(config.broker);
    client.post_records(data->measurements, data->events);

    data->measurements.clear();
    data->events.clear();
}
 */
}
