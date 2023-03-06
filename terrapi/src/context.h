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

    void tick();

    Clock* clock() const;

    Sensor* get_sensor(const std::string& name) const;
    Switch* get_switch(const std::string& name) const;

private:

};

Context& ctx();
}
