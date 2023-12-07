#pragma once

#include <memory>

#include "config.h"

namespace terra
{
class Sensor;
class Switch;

/// Represents the current state of the system.
class Context
{
    friend class Core;

    /// Returns the new context, or nullptr if the context could not be created.
    static std::unique_ptr<Context> create(const Config& config);

public:
    Sensor* get_sensor(const std::string& name) const;
    Switch* get_switch(const std::string& name) const;

    Clock* clock;

    /// @todo consider using a vector instead
    std::map<std::string, std::unique_ptr<Sensor>> sensors;

    /// @todo consider using a vector instead
    std::map<std::string, Switch>                  switches;
};
}
