#include "context.h"

#include <chrono>
#include <memory>
#include <thread>

#include "http.h"
#include "logger.h"
#include "record.h"
#include "sensor.h"
#include "switch.h"

#define TICK_STEP_MS 100

namespace terra
{
static Context* context = nullptr;
static Clock*   ctx_clock = nullptr;

static std::map<std::string, Sensor*> sensors;
static std::map<std::string, Switch*> switches;

static std::vector<MeasurementData> measurements;
static std::vector<EventData>       events;

Context::~Context()
{
    delete ctx_clock;

    for (const auto& [_, sensor] : sensors) {
        delete sensor;
    }
    sensors.clear();

    for (const auto& [_, s] : switches) {
        delete s;
    }
    switches.clear();
}

void Context::create(const Config& config)
{
    if (context != nullptr) {
        log_message("info", "recreating context");
        delete context;
    }

    context = new Context();
    context->config = config;

    ctx_clock = new Clock();

    for (const auto& sensor_config : context->config.sensors) {
        sensors.insert({
            sensor_config.name,
            sensor_types.at(sensor_config.sensor_type)(sensor_config.gpio)
        });
    }

    for (const auto& switch_config : context->config.switches) {
        const auto maybe_expr = Expr::from(switch_config.rule);
        if (!maybe_expr) {
            log_message("error", "invalid rule for switch " + switch_config.name);
            continue;
        }

        switches.insert({
            switch_config.name,
            new Switch((SwitchConfig*) &switch_config, *maybe_expr)
        });
    }
}

void Context::reset()
{
    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();
    context->next_measure_time = time_start_ms + config.environment.measure_step;
    context->next_publish_time = time_start_ms + config.environment.publish_step;
}

void Context::run()
{
    HttpClient client(context->config.broker);
    client.put_config(context->config.raw);

    context->reset();

    tick();
}

void Context::tick()
{
    ctx_clock->measure();

    const auto now = ctx_clock->value();

    auto time_start = std::chrono::high_resolution_clock::now();
    auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();

    if (time_start_ms > next_measure_time) {
        measure(now);
        next_measure_time += config.environment.measure_step;
    }

    update(now);

    if (time_start_ms > next_publish_time) {
        post_records();
        next_publish_time += config.environment.publish_step;
    }

    auto time_end = std::chrono::high_resolution_clock::now();
    auto ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();

    if (ms_elapsed < TICK_STEP_MS) {
        std::chrono::milliseconds sleep_period(TICK_STEP_MS - ms_elapsed);
        std::this_thread::sleep_for(sleep_period);
    } else {
        log_message("warn", "tick step takes " + std::to_string(ms_elapsed));
    }
}

Clock* Context::clock() const
{
    return ctx_clock;
}

Sensor* Context::get_sensor(const std::string& name) const
{
    if (sensors.count(name) == 0) {
        return nullptr;
    }

    return sensors.at(name);
}

Switch* Context::get_switch(const std::string& name) const
{
    if (switches.count(name) == 0) {
        return nullptr;
    }

    return switches.at(name);
}

void Context::measure(Time now)
{
    for (const auto& [name, sensor] : sensors) {
        sensor->measure();

        for (const auto& [type, value] : sensor->measured_values()) {
            measurements.push_back(MeasurementData{
                name.c_str(),
                value,
                (unsigned) type,
                now
            });
        }
    }
}

void Context::update(Time now)
{
    for (const auto& [name, s] : switches) {
        const auto previous_state = s->is_on();
        s->update(now);
        const auto new_state = s->is_on();

        if (previous_state != new_state) {
            events.push_back(EventData{
                name.c_str(),
                new_state,
                now
            });
        }
    }
}

void Context::post_records()
{
    HttpClient client(config.broker);
    client.post_records(measurements, events);

    measurements.clear();
    events.clear();
}

Context& ctx()
{
    return *context;
}
}
