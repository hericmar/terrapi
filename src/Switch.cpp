#include "Switch.hpp"

#include "Defs.h"
#include "Log.h"

using namespace Terra;

Switch::Switch(int GPIO, SensorController* sensorController, int oscillationStep)
{
    this->GPIO = GPIO;

    pinMode(GPIO, OUTPUT);
    Off();
}

void Switch::_SwitchOn()
{
    m_switchedAt = std::clock();

    LOG_DEBUG("Switching ON GPIO %d.\n", GPIO);
    digitalWrite(GPIO, GPIO_ON);
    switched = true;
}

void Switch::_SwitchOff()
{
    LOG_DEBUG("Switching OFF GPIO %d.\n", GPIO);
    digitalWrite(GPIO, GPIO_OFF);
    switched = false;
}
