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

void DummySensor::set_value(float value)
{

}
}
