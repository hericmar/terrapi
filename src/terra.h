#pragma once

#define GPIO_ON     HIGH
#define GPIO_OFF    LOW

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__);
#define LOG_DEBUG(...) printf(__VA_ARGS__);

#define MEASURE_STEP 2000 /// Measure step in milliseconds.

#include "pch.h"
#include "sensor.hpp"
#include "switch.hpp"

namespace Terra
{
    /// Appliction.
    class App
    {
    public:
        App()
        {
        };

        static App& Get();

        PhysicalSensor* GetSensorByGPIO(unsigned gpio)
        {
            for (const auto& sensor : m_physicalSensors)
                if (sensor->GetGPIO() == gpio)
                    return sensor;

            return nullptr;
        }

        Sensor* GetSensorById(unsigned id)
        {
            for (const auto& sensor : m_sensors)
                if (sensor->GetId() == id)
                    return sensor;

            return nullptr;
        }

        std::vector<Sensor*>& GetSensors()
        {
            return m_sensors;
        }

        void PrintSensors();

    private:
        friend class ConfigurationParser;

        static App s_ref;
        std::vector<PhysicalSensor*> m_physicalSensors;
        std::vector<Switch*> m_switches;
        std::vector<Sensor*> m_sensors;
    };
}
