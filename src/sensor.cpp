#include "sensor.hpp"

using namespace Terra;

void Sensor::Update()
{
    m_physicalSensor->Measure();

    float measuredValue = m_physicalSensor->GetValue(m_physicalQuantity);

    if (m_activeInterval.m_from < measuredValue && measuredValue < m_activeInterval.m_to)
    {
        for (auto& aSwitch : m_switches) { aSwitch->On(); }
    }
    else
    {
        for (auto& aSwitch : m_switches) { aSwitch->Off(); }
    }
}
