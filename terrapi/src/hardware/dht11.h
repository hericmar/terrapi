#pragma once

#include "core/sensor.h"

namespace terra
{
class DHT11 : public Sensor
{
public:
    explicit DHT11(int gpio)
        : gpio(gpio)
    {
        values[HUMIDITY];
        values[TEMPERATURE];
    }

    void measure() override;

private:
    int gpio = 0;
};
}
