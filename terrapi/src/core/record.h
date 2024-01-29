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
    float       value;
    ValueType   physical_quantity;
};
}
