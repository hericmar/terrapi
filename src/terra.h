#pragma once

#include <stdio.h>

#include "fmt/format.h"

#include "environment.h"
#include "pch.h"

#define GPIO_ON HIGH
#define GPIO_OFF LOW

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);
#define LOG_DEBUG(...) printf(__VA_ARGS__);

namespace Log
{
template <typename... Args>
void Error(const std::string& fmt, Args&&... args)
{
    fprintf(stderr, "Error: ");
    fprintf(stderr, fmt::format(fmt, std::forward<Args>(args)...).c_str());
    fprintf(stderr, "\n");
}

template <typename... Args>
void Info(const std::string& fmt, Args&&... args)
{
    printf(fmt::format(fmt, std::forward<Args>(args)...).c_str());
    printf("\n");
}
} // namespace Log

static constexpr unsigned int MEASURE_STEP = 2000; /// Measure step in milliseconds.

namespace Terra
{
class PhysicalSensor;
class Sensor;
class Switch;

/// Application.
class App
{
public:
    App() = default;

    static App& Get();

    PhysicalSensor* GetSensorByGPIO(unsigned gpio);

    Sensor* GetSensorById(unsigned id);

    Environment& GetEnvironment() { return m_environment; }

    std::vector<Sensor*>& GetSensors() { return m_sensors; }
    std::vector<PhysicalSensor*>& GetPhysSensors() { return m_physicalSensors; }
    std::vector<Switch*>& GetSwitches() { return m_switches; }

    void PrintSensors();

private:
    static App s_ref;

    Environment m_environment{};

    std::vector<PhysicalSensor*> m_physicalSensors;
    std::vector<Switch*> m_switches;
    std::vector<Sensor*> m_sensors;
};
} // namespace Terra
