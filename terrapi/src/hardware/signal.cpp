// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "hardware/signal.h"

#include "wiringPi.h"

#include "logger.h"

namespace terra
{
void Signal::measure()
{
    const auto liquid_level = digitalRead(gpio);
    // printf("GPIO#%d %d\n", gpio, liquid_level);
    values[SIGNAL] = liquid_level;

    LOG(TRACE, "signal: {}", liquid_level);
}
}
