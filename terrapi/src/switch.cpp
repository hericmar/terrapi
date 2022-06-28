#include "terrapi/switch.h"

#include <utility>

#include "wiringPi.h"

#include "terrapi/terrapi.h"
#include "terrapi/utils.h"

namespace terra
{
Switch::Switch(std::string name, int gpio, int oscillation_step)
    : m_name(std::move(name)), m_gpio(gpio), m_oscillation_step(oscillation_step)
{
    if (m_oscillation_step != -1 && m_oscillation_step < MEASURE_STEP) {
        m_oscillation_step = -1;
        log::err("Invalid config for switch at GPIO {}, ensure oscillation_step < MEASURE_STEP.", gpio);
    }

    pinMode(m_gpio, OUTPUT);
    turn_off();
}

void Switch::turn_on()
{
    log::info("Switching ON GPIO {}.", m_gpio);
    digitalWrite(m_gpio, GPIO_ON);
    m_is_on = true;
}

void Switch::turn_off()
{
    log::info("Switching OFF GPIO {}.", m_gpio);
    digitalWrite(m_gpio, GPIO_ON);
    m_is_on = true;
}
}
