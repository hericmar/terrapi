#include "Terra.h"

#include "ConfParser.h"

using namespace Terra;

App App::s_ref;

App& App::Get() { return s_ref; }

void App::Initialize(const std::string& confFilePath)
{
    printf("=== TerraPi - simple terrarium automation ===\n");
    m_physicalSensors.push_back(&m_clock);

    ConfigurationParser::ReadFile(confFilePath.c_str());
    printf("=== Configuration parsing completed! ===\n");
}

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
SensorController* App::GetSensorById(unsigned int id)
{
    for (const auto& sensor : m_sensors)
        if (sensor->GetId() == id) return sensor;

    return nullptr;
}
Time App::GetTime() { return m_clock.GetValue(EPhysicalQuantityType::Time); }

void App::Run()
{
    while (m_shouldRun)
    {
        for (const auto& sensor : GetPhysSensors())
            sensor->Measure();

        for (const auto& sensorController : GetSensors())
        {
            sensorController->Update();
            delay(MEASURE_STEP);
        }

        PrintSensors();
    }
}
