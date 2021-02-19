#pragma once

#include "pch.h"
#include "switch.hpp"
#include "terra.h"
#include "time_utils.h"

#include <ctime>

#define DHT_MAX_TIMINGS 85

#define GPIO_TIMER 9999

namespace Terra
{
enum class EPhysicalQuantityType
{
    Temperature = 0,
    Humidity,
    Time,
    COUNT
};
constexpr std::array<const char*, (unsigned) EPhysicalQuantityType::COUNT> physicalQuantities{"temperature", "humidity",
                                                                                              "time"};

enum class ESensorType
{
    DHT11 = 0,
    COUNT
};
constexpr std::array<const char*, static_cast<unsigned>(ESensorType::COUNT)> sensors{"DHT11"};


/// Represents a interval in which a switch should be active.
struct ActiveInterval
{
    float m_from = FLT_MIN;
    float m_to   = FLT_MAX;
};

/// Each sensor must have its own physical quantity.
class PhysicalSensor
{
public:
    /// \param gpio wiring pi pin ID.
    explicit PhysicalSensor(int gpio)
    {
        for (auto& value : m_values) value = FLT_MIN; // Set value to default value.

        m_gpio = gpio;
    }

    /// Each sensor has its own way how to perform measurement.
    virtual void Measure() = 0;

    /// Get desired value.
    /// \param type physical quantity.
    float GetValue(EPhysicalQuantityType type) { return m_values[(unsigned) type]; }

    [[nodiscard]] int GetGPIO() const { return m_gpio; };

protected:
    /// Measured values. Each sensor has by default all values measured.
    /// If any value is not supported by the sensor, its value is FLT_MIN.
    std::array<float, (unsigned) EPhysicalQuantityType::COUNT> m_values{};

    /// Set measured value (used in measuring proccess).
    /// \param type value type.
    /// \param value measured value.
    void SetValue(EPhysicalQuantityType type, float value) { m_values[(unsigned) type] = value; }

    /// Data GPIO (WiringPi numbering).
    int m_gpio{};
};

/// Logical sensor.
/// It is used for generic sensor definition.
/// It stores reference to a "physical" sensor and measure one value
/// only.
class Sensor
{
public:
    /// Create a logical representation of the one measured value.
    /// \param physicalQuantity watched value.
    /// \param sensor physical sensor.
    /// \param id unique id!
    explicit Sensor(EPhysicalQuantityType physicalQuantity, class PhysicalSensor* sensor, unsigned id)
    {
        m_physicalSensor   = sensor;
        m_physicalQuantity = physicalQuantity;
        m_id               = id;
    }

    /// Set interval in which assigned switches should be active.
    /// \param from min value.
    /// \param to max value.
    void SetActiveInterval(float from, float to)
    {
        m_activeInterval.m_from = from;
        m_activeInterval.m_to   = to;
    }

    /// Add new switch dependent to measured phys. quantity.
    /// \param aSwitch
    void SetSwitch(class Switch* aSwitch) { m_switches.push_back(aSwitch); }

    /// Perform measurement and update switches.
    void Update();

    [[nodiscard]] unsigned GetId() const { return m_id; };

    /// Sensor has only one measured value!
    [[nodiscard]] float GetValue() const { return m_physicalSensor->GetValue(m_physicalQuantity); }

    [[nodiscard]] EPhysicalQuantityType GetPhysQuantity() const { return m_physicalQuantity; }

private:
    friend class App;

    /// Watched physical quantity.
    EPhysicalQuantityType m_physicalQuantity;

    /// Interval for switch a switch on.
    ActiveInterval m_activeInterval;

    /// PhysicalSensor reference.
    class PhysicalSensor* m_physicalSensor;

    unsigned m_id{};

    /// Switches which depends on the sensor.
    std::vector<class Switch*> m_switches;
};


class DHT11 : public PhysicalSensor
{
public:
    explicit DHT11(int gpio) : PhysicalSensor(gpio){};

    void Measure() override;;

private:
    int data[5] = {0, 0, 0, 0, 0};
};

class Timer : public PhysicalSensor
{
public:
    explicit Timer(int gpio) : PhysicalSensor(gpio){};

    /// Timer measure time ratio.
    void Measure() override
    {
        std::time_t t = std::time(nullptr);
        std::tm* now  = std::localtime(&t);

        float result = TimeToFloat(*now);

        SetValue(EPhysicalQuantityType::Time, result);
    }
};
} // namespace Terra