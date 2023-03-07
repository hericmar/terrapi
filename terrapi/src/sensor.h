#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "datetime.h"

namespace terra
{
using Value = float;

enum ValueType
{
    ValueType_Humidity = 0,
    ValueType_Signal,
    ValueType_Temperature,
    ValueType_None,
};

inline static const std::map<std::string, ValueType> value_type_to_str = {
    { "humidity", ValueType_Humidity },
    { "signal", ValueType_Signal },
    { "temperature", ValueType_Temperature },
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
    int                        gpio;
    std::map<ValueType, Value> values;
};

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

/// Low resolution clock!
class Clock
{
public:
    void measure();

    Time value() const { return m_value; }

    void force_value(Time value) { m_value = value; }

private:
    Time m_value;
};

//----------------------------------------------------------------------------//

using CreateSensorFn  = std::function<Sensor*(int)>;
using CreateSensorMap = std::map<std::string, CreateSensorFn>;

/// Caller takes the ownership.
template <typename T>
Sensor* create_sensor(int gpio)
{
    return new T(gpio);
}

inline static CreateSensorMap sensor_types = {
    { "DHT11", create_sensor<DHT11> },
    { "Signal", create_sensor<Signal> },
    { "Dummy", create_sensor<DummySensor> }
};
}
