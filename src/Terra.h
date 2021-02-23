#pragma once

#include <stdio.h>

#include "Environment.h"
#include "Sensor.hpp"

namespace Terra
{
class PhysicalSensor;
class SensorController;
class Switch;

/// Application.
class App
{
public:
    App() = default;
    static App& Get();

    void Initialize(const std::string& confFilePath);
    void PrintSensors();
    static Switch* CreateSwitch(int gpio, SensorController* sensorController, int oscillationStep);
    static SensorController* CreateTimer(int id, ActiveInterval activeInterval);
    static SensorController* CreateSensorController(int id, EPhysicalQuantityType physicalQuantityType, PhysicalSensor* physicalSensor);
    static PhysicalSensor* CreateSensor(ESensorType type, int gpio);
    PhysicalSensor* GetSensorByGPIO(unsigned gpio);
    SensorController* GetSensorById(unsigned id);
    Environment& GetEnvironment() { return m_environment; }
    std::vector<SensorController*>& GetSensors() { return m_sensors; }
    std::vector<PhysicalSensor*>& GetPhysSensors() { return m_physicalSensors; }
    std::vector<Switch*>& GetSwitches() { return m_switches; }

    Clock* GetClock() { return &m_clock; };
    Time GetTime();

    void Run();

private:
    static App s_ref;

    bool m_shouldRun = true;
    Environment m_environment{};
    Clock m_clock;

    std::vector<PhysicalSensor*> m_physicalSensors;
    std::vector<Switch*> m_switches;
    std::vector<SensorController*> m_sensors;
};
} // namespace Terra
