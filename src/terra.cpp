#include "terra.h"

using namespace Terra;

App App::s_ref;

App& App::Get()
{
    return s_ref;
}

void App::PrintSensors()
{
    printf("\n=== Sensor stats ===\n");
    for (const auto& sensor : m_sensors)
    {
        printf("ID %d:\t", sensor->GetId());
        printf("%s %f\n", physicalQuantities[(unsigned) sensor->GetPhysQuantity()], sensor->GetValue());
    }
    printf("====================\n\n");
}
