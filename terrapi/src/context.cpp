#include "context.h"

#include <chrono>
#include <thread>

#include "http.h"
#include "logger.h"
#include "record.h"
#include "sensor.h"
#include "switch.h"

#define TICK_STEP_MS 100

namespace terra
{
/// Singleton instance of the context.
static Context* context = nullptr;

class ContextData
{
public:


    Clock clock{};

    std::map<std::string, std::unique_ptr<Sensor>> sensors;
    std::map<std::string, Switch> switches;

    std::vector<MeasurementData> measurements;
    std::vector<EventData>       events;
};

Context::~Context()
{
    delete data;
}

Context* Context::create(const Config& config)
{
    if (context != nullptr) {
        log_message(WARN, "recreating context");
        delete context;
    }

    context = new Context();
    context->config = config;

    auto* self = new ContextData();
    context->data = self;

    for (const auto &sensor_config: context->config.sensors) {
        self->sensors.insert({
            sensor_config.name,
            sensor_factory.at(sensor_config.sensor_type)(sensor_config.gpio)
        });
    }

    for (const auto& switch_config : context->config.switches) {
        const auto maybe_expr = Expr::from(switch_config.rule);
        if (!maybe_expr) {
            log_message(ERR, "invalid rule for switch " + switch_config.name);
            continue;
        }

        self->switches.insert({
            switch_config.name,
            Switch((SwitchConfig*) &switch_config, *maybe_expr)
        });
    }

    return context;
}

void Context::reset()
{
    const auto time_start = std::chrono::system_clock::now();
    const auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch())
            .count();

    next_measure_time = time_start_ms;
    next_publish_time = time_start_ms;
}

void Context::run()
{
    HttpClient client(config.broker);
    client.put_config(config.raw);

    reset();

    while (true) {
        tick();
    }
}

void Context::tick()
{
    data->clock.measure();

    const auto now = data->clock.value();

    const auto time_start = std::chrono::system_clock::now();
    const auto time_start_ms = std::chrono::duration_cast<std::chrono::milliseconds>(time_start.time_since_epoch()).count();

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

    const auto time_end = std::chrono::system_clock::now();
    const auto ms_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();

    if (ms_elapsed < TICK_STEP_MS) {
        std::chrono::milliseconds sleep_period(TICK_STEP_MS - ms_elapsed);
        std::this_thread::sleep_for(sleep_period);
    } else {
        log_message(WARN, "tick step takes " + std::to_string(ms_elapsed));
    }
}

Clock& Context::clock() const
{
    return data->clock;
}

Sensor* Context::get_sensor(const std::string& name) const
{
    if (data->sensors.count(name) == 0) {
        return nullptr;
    }

    return data->sensors.at(name).get();
}

Switch* Context::get_switch(const std::string& name) const
{
    if (data->switches.count(name) == 0) {
        return nullptr;
    }

    return &data->switches.at(name);
}

void Context::measure(Time now, uint64_t timestamp)
{
    const bool should_publish = now >= next_publish_time;

    for (const auto& [name, sensor] : data->sensors) {
        sensor->measure();

        if (should_publish) {
            for (const auto& [type, value] : sensor->measured_values()) {
                data->measurements.push_back(MeasurementData{
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

void Context::post_records()
{
    HttpClient client(config.broker);
    client.post_records(data->measurements, data->events);

    data->measurements.clear();
    data->events.clear();
}

Context& curr_ctx()
{
    assert(context != nullptr);

    return *context;
}
}
