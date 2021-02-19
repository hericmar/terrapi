#include "terra.h"

#include "sensor.hpp"
#include "switch.hpp"

using namespace Terra;

App App::s_ref;

App& App::Get() { return s_ref; }

void App::PrintSensors()
{
    printf("\n===== Sensor stats =====\n");
    for (const auto& sensor : m_sensors)
    {
        printf("ID %d:\t", sensor->GetId());
        printf("%s %.4f\n", physicalQuantities[(unsigned) sensor->GetPhysQuantity()], sensor->GetValue());

        for (const auto& aSwitch : sensor->m_switches)
        {
            printf("\tGPIO: %d - %d\n", aSwitch->GetGPIO(), aSwitch->IsSwitched());
        }
    }
    printf("========================\n\n");
}

PhysicalSensor* App::GetSensorByGPIO(unsigned int gpio)
{
    for (const auto& sensor : m_physicalSensors)
        if (sensor->GetGPIO() == gpio) return sensor;

    return nullptr;
}
Sensor* App::GetSensorById(unsigned int id)
{
    for (const auto& sensor : m_sensors)
        if (sensor->GetId() == id) return sensor;

    return nullptr;
}
