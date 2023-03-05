#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace terra
{
using Value = float;

enum ValueType
{
    ValueType_Humidity = 0,
    ValueType_Signal,
    ValueType_Temperature,
    ValueType_Time,  /// time is reserved name
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

    bool measure_value(ValueType type) const { return values.count(type) != 0; }

protected:
    std::map<ValueType, Value> values;
};

class DHT11 : public Sensor
{

};

class Signal : public Sensor
{

};

class Clock : public Sensor
{

};

class DummySensor : public Sensor
{
public:
    DummySensor()
    {
        values[ValueType_Humidity];
        values[ValueType_Signal];
        values[ValueType_Temperature];
    }

    void set_value(float value);
};

//----------------------------------------------------------------------------//

using CreateSensorFn  = std::function<Sensor*()>;
using CreateSensorMap = std::map<std::string, CreateSensorFn>;

/// Caller takes the ownership.
template <typename T>
Sensor* create_sensor()
{
    return new T;
}

inline static CreateSensorMap sensor_types = {
    { "DHT11", create_sensor<DHT11> },
    { "Signal", create_sensor<Signal> },
    { "Dummy", create_sensor<DummySensor> }
};
}
