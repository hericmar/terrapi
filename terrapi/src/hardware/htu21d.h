#pragma once

#include "core/sensor.h"

#define HTU21D_I2C_ADDR 0x40

#define HTU21D_TEMP     0xF3
#define HTU21D_HUMID    0xF5

namespace terra
{
/// Note that this sensor has only one address.
class HTU21D : public Sensor
{
public:
    HTU21D();

    void measure() override;

private:
    int fd = 0;
};
}
