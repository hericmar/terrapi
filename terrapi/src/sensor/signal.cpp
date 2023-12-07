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

    log_message(TRACE, "signal: " + std::to_string(liquid_level));
}
}
