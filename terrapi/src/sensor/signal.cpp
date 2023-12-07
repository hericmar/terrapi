#include "sensor/signal.h"

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
