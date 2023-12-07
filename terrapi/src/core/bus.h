#pragma once

#include <stdint.h>
#include <variant>

#include "core/sensor.h"
#include "switch.h"

namespace terra
{
struct Event
{
    const char* src;
    uint64_t    timestamp;
};

struct EventMeasurement : Event
{
    float     value;
    ValueType physical_quantity;
};

struct EventSwitch : Event
{
    SwitchState state;
};

using EventType = std::variant<EventMeasurement, EventSwitch>;

/// Bus is a message bus.
class Bus
{
public:
    /// Send all events to the server.
    void publish();

    /// Push an event to the bus.
    void record(EventType event);

private:
    std::vector<EventType> events;
};
}
