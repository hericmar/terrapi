#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "datetime.h"

namespace terra
{
using Value = float;

/// Insert new value types at the end of the enum and update the map value_type_to_str.
enum ValueType
{
    ValueType_Humidity = 0,
    ValueType_Signal,
    ValueType_Temperature,
    ValueType_Time,
    ValueType_None,
};

inline static const std::map<std::string, ValueType> value_type_to_str = {
    { "humidity", ValueType_Humidity },
    { "signal", ValueType_Signal },
    { "temperature", ValueType_Temperature },
    { "time", ValueType_Time },
};

class Sensor
{
public:
    virtual void measure() {}
    Value value(ValueType type);

    void force_value(ValueType type, float value);

    bool measures_value(ValueType type) const { return values.count(type) != 0; }

    const std::map<ValueType, Value>& measured_values() { return values; }

protected:
    int                        gpio = -1;
    std::map<ValueType, Value> values;
};

using SensorMap = std::map<std::string, std::unique_ptr<Sensor>>;

class DHT11 : public Sensor
{
public:
    DHT11(int gpio)
    {
        this->gpio = gpio;
        values[ValueType_Humidity];
        values[ValueType_Temperature];
    }

    void measure() override;
};

class Signal : public Sensor
{
public:
    Signal(int gpio)
    {
        this->gpio = gpio;
        values[ValueType_Signal];
    }

    void measure() override;
};

class DummySensor : public Sensor
{
public:
    DummySensor(int gpio)
    {
        values[ValueType_Humidity];
        values[ValueType_Signal];
        values[ValueType_Temperature];
    }
};

/// Logical clock.
///
/// Has time value in seconds, with 0 being the start of the day.
class Clock : public Sensor
{
public:
    Clock()
    {
        values[ValueType_Time];
    }

    void measure() override;

    uint64_t value() const { return m_value; }

    void force_value(uint64_t value)
    {
        m_value = value;
        values[ValueType_Time] = (float) m_value;
    }

private:
    uint64_t m_value = 0;
};

//----------------------------------------------------------------------------//

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
