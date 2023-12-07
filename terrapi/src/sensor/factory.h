#pragma once

#include "sensor/dht11.h"
#include "sensor/signal.h"

namespace terra
{
// Sensor factory

using CreateSensorFn = std::function<std::unique_ptr<Sensor>(int)>;

/// Caller takes the ownership.
template <typename T>
auto create_sensor(int gpio)
{
    return std::make_unique<T>(gpio);
}

inline static std::map<std::string, CreateSensorFn> sensor_factory = {
        { "DHT11", create_sensor<DHT11> },
        { "Signal", create_sensor<Signal> },
        { "Dummy", create_sensor<DummySensor> }
};
}
