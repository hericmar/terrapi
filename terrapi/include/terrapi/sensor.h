#pragma once

#include <array>
#include <ctime>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

namespace terra
{
enum class EPhysicalQuantity
{
    Humidity    = 0,
    Temperature,
    Time,
};

const std::string& to_string(EPhysicalQuantity q);
std::optional<EPhysicalQuantity> from_string(const std::string& str);

using ValueInterval = std::array<float, 2>;

using SensorPtr = std::unique_ptr<class PhysicalSensor>;

//------------------------------------------------------------------------------

/// Physical sensor can measure.
class PhysicalSensor
{
    using Values = std::unordered_map<EPhysicalQuantity, float>;

public:
    virtual void measure(const std::tm& now) = 0;

    [[nodiscard]] const std::string& name() const { return m_name; }

    float value(EPhysicalQuantity q) const;
    [[nodiscard]] const Values& values() const { return m_value; }

protected:
    std::string m_name;
    Values      m_value;
};

//------------------------------------------------------------------------------

class Clock : public PhysicalSensor
{
public:
    Clock();

    void measure(const std::tm& now) override;

    bool is_day() const;
};

//------------------------------------------------------------------------------

#define DHT_MAX_TIMINGS 85

class DHT11 : public PhysicalSensor
{
public:
    /// \todo Validate GPIO number
    explicit DHT11(int gpio) : m_gpio(gpio) {
        m_name = "DHT11@" + std::to_string(gpio);

        m_value[EPhysicalQuantity::Humidity];
        m_value[EPhysicalQuantity::Temperature];
    }

    void measure(const std::tm& now) override;

private:
    int m_gpio;
    int m_data[5]{};
};

//------------------------------------------------------------------------------

}
