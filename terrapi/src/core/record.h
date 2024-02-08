#pragma once

#include <stdint.h>
#include <variant>

#include "core/sensor.h"
#include "core/switch.h"

namespace terra
{
struct BaseRecord
{
    const char* src;
    /// In milliseconds, converted to seconds when sent to the server.
    uint64_t    timestamp;
};

struct Event : public BaseRecord
{
    int state;
};

struct Measurement : public BaseRecord
{
    float     value;
    ValueType physical_quantity;
};

using Record = std::variant<Event, Measurement>;
}
