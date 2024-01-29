#pragma once

#include <memory>
#include <vector>

#include "core/bus.h"
#include "core/context.h"
#include "core/timer.h"
#include "config.h"

namespace terra
{
/// The core of the application.
class Core
{
    explicit Core(Config config);

public:
    /// @pre Configuration is valid!
    /// @returns Non-owned pointer to the created core, or nullptr if failed.
    static Core* create(Config config);

    /// Starts the infinite loop.
    void run();

    /// Called periodically with a fixed interval.
    void tick();

    /// Setup initial state.
    void reset();

    void shutdown();

    void record_event(Record record);

    Context& context();

private:
    std::unique_ptr<Context> ctx = nullptr;
    std::unique_ptr<Bus>     bus;
    std::vector<Timer>       timers;

public:
    const Config config;
};

Core& core();
}
