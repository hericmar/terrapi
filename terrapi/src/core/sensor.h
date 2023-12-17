#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace terra
{
using Value = float;

/// Insert new value types at the end of the enum and update the map value_type_to_str.
enum ValueType
{
    HUMIDITY = 0,
    SIGNAL,
    TEMPERATURE,
    TIME,
    NONE,
};

inline static const std::map<std::string, ValueType> value_type_to_str = {
    { "humidity", HUMIDITY },
    { "signal", SIGNAL },
    { "temperature", TEMPERATURE },
    { "time", TIME },
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
    std::map<ValueType, Value> values;
};

using SensorMap = std::map<std::string, std::unique_ptr<Sensor>>;

class DummySensor : public Sensor
{
public:
    explicit DummySensor(int gpio)
    {
        values[HUMIDITY];
        values[SIGNAL];
        values[TEMPERATURE];
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
        values[TIME];
    }

    /// Measure current time in seconds, based on the system TZ.
    void measure() override;

    void force_value(uint64_t value)
    {
        m_value = value;
        values[TIME] = (float) m_value;
    }

private:
    uint64_t m_value = 0;
};
}
