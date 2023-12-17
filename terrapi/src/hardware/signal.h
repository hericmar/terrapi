#pragma once

#include "core/sensor.h"

namespace terra
{
class Signal : public Sensor
{
public:
    explicit Signal(int gpio)
    {
        this->gpio = gpio;
        values[SIGNAL];
    }

    void measure() override;
};
}
