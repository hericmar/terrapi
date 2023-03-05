#include "context.h"

#include <memory>

#include "logger.h"
#include "switch.h"

namespace terra
{
static Context* context = nullptr;
static Clock*   ctx_clock = nullptr;

static std::map<std::string, Sensor*> sensors;
static std::map<std::string, Switch*> switches;

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

    ctx_clock = new Clock();

    for (const auto& sensor_config : config.sensors) {
        sensors.insert({
            sensor_config.name,
            sensor_types.at(sensor_config.sensor_type)()
        });
    }

    for (const auto& switch_config : config.switches) {
        switches.insert({
            switch_config.name,
            new Switch(switch_config.rule)
        });
    }

    context = new Context();
}

Sensor* Context::clock() const
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

const Context& ctx()
{
    return *context;
}
}
