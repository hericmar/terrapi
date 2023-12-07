#include <chrono>
#include <thread>
#include "core/core.h"

#include "core/context.h"
#include "config.h"
#include "logger.h"
#include "switch.h"

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
    assert(ker == nullptr);
}

Core *Core::create(Config config)
{
    if (ker != nullptr) {
        log_message(WARN, "recreating current context");
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
        log_message(TRACE, "tick step takes " + std::to_string(elapsed) + " ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(TICK_STEP_MS - elapsed));
    }
}

//

void measure()
{
    log_message(TRACE, "measure");

    auto& ctx = core().context();

    for (auto& [name, sensor] : ctx.sensors) {
        log_message(TRACE, "measuring " + name);
        sensor->measure();
    }
}

void update()
{
    log_message(TRACE, "update");

    auto& ctx = core().context();
    const auto now_ms = ctx.clock->value();

    for (auto& [name, s] : ctx.switches) {
        s.update(now_ms);
    }
}

void Core::reset()
{
    timers.clear();

    timers.emplace_back(config.environment.measure_step, measure);
    timers.emplace_back(TICK_STEP_MS, update);
}

void Core::record_event(EventType event)
{
    bus.record(event);
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
