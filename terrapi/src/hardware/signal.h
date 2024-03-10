// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#pragma once

#include "core/sensor.h"

namespace terra
{
class Signal : public Sensor
{
public:
    explicit Signal(int gpio)
        : gpio(gpio)
    {
        values[SIGNAL];
    }

    void measure() override;

private:
    int gpio = 0;
};
}
