#pragma once

#include <stdint.h>
#include <variant>

#include "core/sensor.h"
#include "core/switch.h"

namespace terra
{
struct Record
{
    const char* src;
    uint64_t    timestamp;
};

struct MeasurementRecord : Record
{
    float     value;
    ValueType physical_quantity;
};

struct EventRecord : Record
{
    SwitchState state;
};

using EventType = std::variant<MeasurementRecord, EventRecord>;
}
