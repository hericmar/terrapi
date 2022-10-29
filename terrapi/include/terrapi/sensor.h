#pragma once

#include <array>
#include <ctime>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

#include <float.h>

namespace terra
{
enum class EPhysicalQuantity
{
    Humidity    = 0,
    Temperature,
    Time,
    Signal
};

const std::string& to_string(EPhysicalQuantity q);
std::optional<EPhysicalQuantity> from_string(const std::string& str);

using ValueInterval = std::array<float, 2>;

using SensorPtr = std::unique_ptr<class PhysicalSensor>;

constexpr const float NONE_VALUE = FLT_MIN;

//------------------------------------------------------------------------------

static inline const std::map<std::string, std::set<EPhysicalQuantity>> g_known_sensors = {
    {"DHT11", {EPhysicalQuantity::Temperature, EPhysicalQuantity::Humidity}},
    {"WaterLevel", {EPhysicalQuantity::Signal}}
};

//------------------------------------------------------------------------------

/// Physical sensor can measure.
class PhysicalSensor
{
    using Values = std::unordered_map<EPhysicalQuantity, float>;

public:
    PhysicalSensor(std::string type) : m_type(std::move(type)) {}

    virtual void measure(const std::tm& now) = 0;

    [[nodiscard]] const std::string& name() const { return m_name; }

    float value(EPhysicalQuantity q) const;
    [[nodiscard]] const Values& values() const { return m_value; }

    const std::set<EPhysicalQuantity>& get_sensor_properties();

protected:
    const std::string m_type;

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
    DHT11(std::string name, int gpio);

    void measure(const std::tm& now) override;

private:
    int m_gpio;
    int m_data[5]{};
};

//------------------------------------------------------------------------------

class WaterLevel : public PhysicalSensor
{
public:
    WaterLevel(std::string name, int gpio);

    void measure(const std::tm& now) override;

private:
    int m_gpio;
};

//------------------------------------------------------------------------------

}
