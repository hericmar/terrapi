#include "Terra.h"

#include "ConfParser.h"
#include "Defs.h"
#include "Sensor.hpp"

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

Switch* App::CreateSwitch(int gpio, SensorController* sensorController, int oscillationStep)
{
    auto aSwitch = new Switch(gpio, sensorController, oscillationStep);
    App::Get().GetSwitches().push_back(aSwitch);

    return aSwitch;
}

SensorController* App::CreateTimer(int id, ActiveInterval activeInterval)
{
    auto* timer = new SensorController(EPhysicalQuantityType::Time, &s_ref.m_clock, id);
    timer->SetActiveInterval(activeInterval);
    s_ref.m_sensors.push_back(timer);

    return timer;
}

SensorController* App::CreateSensorController(int id, EPhysicalQuantityType physicalQuantity,
                                              PhysicalSensor* physSens)
{
    auto* sensor = new SensorController(physicalQuantity, physSens, id);
    s_ref.m_sensors.push_back(sensor);

    return sensor;
}

PhysicalSensor* App::CreateSensor(ESensorType type, int gpio)
{
    PhysicalSensor* physSens = nullptr;

    switch (type)
    {
    case ESensorType::DHT11:
    {
        physSens = new DHT11(gpio);
        break;
    }
    default:
        return nullptr;
    }
    s_ref.m_physicalSensors.push_back(physSens);

    return physSens;
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
