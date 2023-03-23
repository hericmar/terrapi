#include "context.h"

#include <chrono>
#include <iostream>
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

class ContextData
{
public:
    ~ContextData()
    {
        for (const auto& [_, sensor] : sensors) {
            delete sensor;
        }
        sensors.clear();

        for (const auto& [_, s] : switches) {
            delete s;
        }
        switches.clear();
    }

    Clock clock{};

    std::map<std::string, Sensor*> sensors;
    std::map<std::string, Switch*> switches;

    std::vector<MeasurementData> measurements;
    std::vector<EventData>       events;
};

Context::~Context()
{
    delete self;
}

void Context::create(const Config& config)
{
    if (context != nullptr) {
        log_message("info", "recreating context");
        delete context;
    }

    context = new Context();
    context->config = config;

    auto* self = new ContextData();
    context->self = self;

    for (const auto& sensor_config : context->config.sensors) {
        self->sensors.insert({
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

        self->switches.insert({
            switch_config.name,
            new Switch((SwitchConfig*) &switch_config, *maybe_expr)
        });
    }
}

void Context::reset()
{
    auto time_start = std::chrono::system_clock::now();
    auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();
    context->next_measure_time = time_start_ms;
    context->next_publish_time = time_start_ms;
}

void Context::run()
{
    HttpClient client(context->config.broker);
    client.put_config(context->config.raw);

    context->reset();

    while (true) {
        tick();
    }
}

void Context::tick()
{
    self->clock.measure();

    const auto now = self->clock.value();

    auto time_start = std::chrono::system_clock::now();
    auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();

    if (time_start_ms >= next_measure_time) {
        measure(now, time_start_ms);
        next_measure_time += config.environment.measure_step;
    }

    update(now, time_start_ms);

    if (time_start_ms >= next_publish_time) {
        if (!config.broker.address.empty()) {
            post_records();
        }
        next_publish_time += config.environment.publish_step;
    }

    auto time_end = std::chrono::system_clock::now();
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
    return &self->clock;
}

Sensor* Context::get_sensor(const std::string& name) const
{
    if (self->sensors.count(name) == 0) {
        return nullptr;
    }

    return self->sensors.at(name);
}

Switch* Context::get_switch(const std::string& name) const
{
    if (self->switches.count(name) == 0) {
        return nullptr;
    }

    return self->switches.at(name);
}

void Context::measure(Time now, uint64_t timestamp)
{
    auto should_publish = now >= next_publish_time;

    for (const auto& [name, sensor] : self->sensors) {
        sensor->measure();

        if (should_publish) {
            for (const auto& [type, value] : sensor->measured_values()) {
                self->measurements.push_back(MeasurementData{
                    name.c_str(),
                    value,
                    (unsigned) type,
                    timestamp / 1000
                });
            }
        }
    }
}

void Context::update(Time now, uint64_t timestamp)
{
    for (const auto& [name, s] : self->switches) {
        const auto previous_state = s->is_on();
        s->update(now);
        const auto new_state = s->is_on();

        if (previous_state != new_state) {
            self->events.push_back(EventData{
                name.c_str(),
                new_state,
                timestamp / 1000
            });
        }
    }
}

void Context::post_records()
{
    HttpClient client(config.broker);
    client.post_records(self->measurements, self->events);

    self->measurements.clear();
    self->events.clear();
}

Context& ctx()
{
    return *context;
}
}
