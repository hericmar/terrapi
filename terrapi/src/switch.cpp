#include "terrapi/switch.h"

#include <iostream>
#include <utility>

#include "wiringPi.h"

#include "terrapi/terrapi.h"
#include "terrapi/utils.h"

namespace terra
{
Switch::Switch(std::string name, int gpio, Expr rules, int oscillation_step)
    : m_name(std::move(name)), m_gpio(gpio), m_rules(std::move(rules)), m_oscillation_step(oscillation_step)
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

    float k = (float) m_elapsed_ms / (float) m_oscillation_step;

    if ((int) std::floor(k) % 2 == 0) {
        if (!m_is_upper) {
            // log::info("{}: UP GPIO {}.", m_name, m_gpio);

            digitalWrite(m_gpio, GPIO_ON);
            m_is_upper = true;
        }
    } else {
        if (m_is_upper) {
            // log::info("{}: DOWN GPIO {}.", m_name, m_gpio);

            digitalWrite(m_gpio, GPIO_OFF);
            m_is_upper = false;
        }
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

void Switch::update(int delta_ms)
{
    if (m_oscillation_step == -1) {
        return;
    }

    /*
    float k = (float) m_elapsed_ms / (float) m_oscillation_step;

    std::cerr << (int) std::floor(k) << std::endl;
    if ((int) std::floor(k) % 2 == 0) {
        turn_on();
    } else {
        turn_off();
    }
     */

    m_elapsed_ms += (unsigned) delta_ms;
}
}
