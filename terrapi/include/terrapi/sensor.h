#pragma once

#include <array>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

namespace terra
{
enum class EPhysicalQuantity
{
    Humidity,
    Temperature,
    Time,
};

/// \todo We have no value type validation.
union Value
{
    float float_val;  // humidity or temperature
    int   int_val;    // time
    bool  bool_val;   //
};

using ValueInterval = std::array<Value, 2>;

using SensorPtr = std::unique_ptr<class PhysicalSensor>;

/// \returns true when lhs < rhs
bool cmp(EPhysicalQuantity q, Value lhs, Value rhs);

bool cmp_within(EPhysicalQuantity q, ValueInterval interval, Value value);

//------------------------------------------------------------------------------

class SensorController
{
public:
    /// For timer only.
    SensorController(
        const std::string& name,
        SensorPtr& sensor,
        EPhysicalQuantity q,
        std::optional<ValueInterval> day)
        : m_name(name), m_sensor(sensor), m_q(q), m_day_interval(day) {}

    SensorController(
        const std::string& name,
        SensorPtr& sensor,
        EPhysicalQuantity q,
        std::optional<ValueInterval> day,
        std::optional<ValueInterval> night)
        : m_name(name), m_sensor(sensor), m_q(q), m_day_interval(day), m_night_interval(night) {}

    void update() const;

    void set_switch_id(int id) { m_switch_idx = id; }

private:
    void evaluate(std::optional<ValueInterval> interval) const;

    std::string       m_name;
    SensorPtr&        m_sensor;
    int               m_switch_idx = -1;
    EPhysicalQuantity m_q;

    std::optional<ValueInterval> m_day_interval{};
    std::optional<ValueInterval> m_night_interval{};
};

//------------------------------------------------------------------------------

/// Physical sensor can measure.
class PhysicalSensor
{
public:
    virtual void measure() = 0;

    Value value(EPhysicalQuantity q);

protected:
    std::unordered_map<EPhysicalQuantity, Value> m_value;
};

//------------------------------------------------------------------------------

class Clock : public PhysicalSensor
{
public:
    Clock();

    void measure() override;

    bool is_day() const;
};

//------------------------------------------------------------------------------

#define DHT_MAX_TIMINGS 85

class DHT11 : public PhysicalSensor
{
public:
    /// \todo Validate GPIO number
    explicit DHT11(int gpio) : m_gpio(gpio) {}

    void measure() override;

private:
    int m_gpio;
    int m_data[5]{};
};

//------------------------------------------------------------------------------

}