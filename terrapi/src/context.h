#pragma once

#include "config.h"

namespace terra
{
class Sensor;
class Switch;

class Context
{
private:
    Context() = default;
    ~Context();

public:
    /// @pre Configuration is validated!
    static void create(const Config& config);

    void reset();

    void run();
    void tick();

    Clock* clock() const;

    Sensor* get_sensor(const std::string& name) const;
    Switch* get_switch(const std::string& name) const;

private:
    /// @param now logical time
    /// @param timestamp
    void measure(Time now, uint64_t timestamp);

    /// @param now logical time
    void update(Time now, uint64_t timestamp);

    void post_records();

    /// ms
    uint64_t next_measure_time = 0;

    /// ms
    uint64_t next_publish_time = 0;

    Config config;
};

Context& ctx();
}
