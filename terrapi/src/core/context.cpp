#include "context.h"

#include <chrono>
#include <thread>

#include "core/core.h"
#include "core/timer.h"
#include "http.h"
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

/*
void Context::run()
{
    HttpClient client(config.broker);
    client.put_config(config.raw);

    reset();

    while (true) {
        tick();
    }
}
 */

/*
void Context::tick()
{
    data->clock.measure();

    const auto now = data->clock.value();

    const auto time_start = std::chrono::system_clock::now();
    const auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();

    //
    if (time_start_ms >= next_measure_time) {
        measure(now, time_start_ms);
        next_measure_time += config.environment.measure_step;
    }
    //
    timer_measure.tick(now);

    update(now, time_start_ms);

    if (time_start_ms >= next_publish_time) {
        if (!config.broker.address.empty()) {
            post_records();
        }
        next_publish_time += config.environment.publish_step;
    }

    const auto time_end = std::chrono::system_clock::now();
    const auto ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();

    if (ms_elapsed < TICK_STEP_MS) {
        std::chrono::milliseconds sleep_period(TICK_STEP_MS - ms_elapsed);
        std::this_thread::sleep_for(sleep_period);
    } else {
        log_message(WARN, "tick step takes " + std::to_string(ms_elapsed));
    }
}
*/

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
void Context::update(Time now, uint64_t timestamp)
{
    for (auto& [name, s] : data->switches) {
        const auto previous_state = s.is_on();
        s.update(now);
        const auto new_state = s.is_on();

        if (previous_state != new_state) {
            data->events.push_back(EventData{
                name.c_str(),
                new_state,
                timestamp / 1000
            });
        }
    }
}
 */

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
