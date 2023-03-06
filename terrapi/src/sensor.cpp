#include "sensor.h"

#include "logger.h"

namespace terra
{
Value Sensor::value(ValueType type)
{
    if (values.count(type) == 0) {
        log_message("error", "invalid value");

        return 0;
    }

    return values.at(type);
}

void Sensor::force_value(ValueType type, float value)
{
    log_message("info", "forcing value");
    values[type] = value;
}
}
