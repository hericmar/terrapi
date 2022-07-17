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
    if (m_oscillation_step != -1 && m_oscillation_step < UPDATE_STEP) {
        m_oscillation_step = -1;
        log::err("Invalid config for switch at GPIO {}, ensure oscillation_step < MEASURE_STEP.", gpio);
    }

    pinMode(m_gpio, OUTPUT);
    turn_off();
}

bool Switch::turn_on()
{
    if (!m_is_on) {
        log::info("{}: Switching ON GPIO {}.", m_name, m_gpio);
        digitalWrite(m_gpio, GPIO_ON);
        m_is_on = true;

        return true;
    }

    return false;
}

bool Switch::turn_off()
{
    if (m_is_on) {
        log::info("{}: Switching OFF GPIO {}.", m_name, m_gpio);
        digitalWrite(m_gpio, GPIO_ON);
        m_is_on = false;

        return true;
    }

    return false;
}
}
