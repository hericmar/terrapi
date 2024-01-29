#include <chrono>
#include <thread>

#include "wiringPi.h"

#include "core/core.h"
#include "core/context.h"
#include "config.h"
#include "logger.h"
#include "switch.h"
#include "http.h"

#define TICK_STEP_MS 100

namespace terra
{
static Core* ker = nullptr;

//

uint64_t get_now_ms()
{
    const auto now_chrono = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<std::chrono::milliseconds>(now_chrono)
            .time_since_epoch()
            .count();
}

//

Core::Core(Config config)
    : config(std::move(config))
{
    bus = std::make_unique<Bus>(this->config.broker);
    assert(ker == nullptr);
}

Core *Core::create(Config config)
{
    if (wiringPiSetup() != 0) {
        LOG(FATAL, "cannot setup Wiring PI");
        return nullptr;
    }

    if (ker != nullptr) {
        LOG(WARN, "recreating current context");
        delete ker;
        ker = nullptr;
    }

    ker = new Core(std::move(config));
    assert(ker != nullptr);

    ker->ctx = Context::create(ker->config);
    ker->reset();

    return ker;
}

void Core::run()
{
    if (config.broker.enabled)
    {
        auto http_client = HttpClient(config.broker);
        http_client.put_client_hello(config.raw);
    }

    while (true) {
        /// @todo interrupt handling
        tick();
    }
}

void Core::tick()
{
    const auto now_ms = get_now_ms();
    uint64_t elapsed = 0;

    for (auto& timer : timers) {
        auto start = get_now_ms();

        timer.tick(now_ms);

        auto end = get_now_ms();
        elapsed += end - start;
    }

    if (elapsed < TICK_STEP_MS) {
        LOG(DEBUG, "tick step takes {} ms", std::to_string(elapsed));
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_STEP_MS - elapsed));
    }
}

//

void measure(uint64_t now)
{
    LOG(TRACE, "measure");

    auto& ctx = core().context();

    for (auto& [name, sensor] : ctx.sensors) {
        LOG(TRACE, "measuring '{}'", name);
        sensor->measure();
    }
}

void update(uint64_t now_ms)
{
    LOG(TRACE, "update");

    auto& ctx = core().context();

    for (auto& [name, s] : ctx.switches) {
        s.update(now_ms);
    }
}

void Core::reset()
{
    timers.clear();

    timers.emplace_back(config.environment.measure_step, measure);
    timers.emplace_back(TICK_STEP_MS, update);
    timers.emplace_back(config.environment.publish_step, [&](uint64_t now) {
        LOG(TRACE, "publish");

        for (const auto& [name, sensor] : ctx->sensors) {
            // skip clock
            if (sensor->measured_values().count(TIME)) {
                continue;
            }

            for (const auto& [type, value] : sensor->measured_values()) {
                bus->record(Record{
                        name.c_str(),
                        now,
                        value,
                        type,
                });
            }
        }

        bus->publish();
    });
}

void Core::shutdown()
{
    for (auto& [_, s] : ctx->switches) {
        s.switch_off();
    }
    bus->publish();
}

void Core::record_event(Record record)
{
    bus->record(record);
}

Context& Core::context()
{
    return *ctx;
}

Core& core()
{
    assert(ker != nullptr);
    return *ker;
}
}
