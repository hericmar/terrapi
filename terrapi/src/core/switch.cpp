#include "switch.h"

#include "wiringPi.h"

#include "config.h"
#include "logger.h"
#include "core/core.h"

namespace terra
{
Switch::Switch(SwitchConfig* config, const Expr& expr)
    : config(config), rule(expr)
{
    pinMode(config->gpio, OUTPUT);
    write_off();

    if (config->oscillation_high == -1 || config->oscillation_low == -1) {
        oscillate = false;
    } else {
        oscillate = true;
    }
}

void Switch::update(uint64_t time)
{
    const auto previous_state = state;
    // Test if the switch should be on or off.
    const auto new_state = SwitchState(rule->evaluate());

    if (new_state) {
        // first switch
        if (state == SWITCH_OFF && oscillate) {
            next_toggle = time + config->oscillation_low;
        }

        switch_on();
    } else {
        switch_off();
    }

    if (state == SWITCH_ON && oscillate) {
        // Is already on, check if it's time to toggle.
        if (time >= next_toggle) {
            // toggle
            if (is_high) {
                write_off();
                next_toggle = next_toggle + config->oscillation_low;
            } else {
                write_on();
                next_toggle = next_toggle + config->oscillation_high;
            }
        }
    }

    if (previous_state != new_state) {
        core().record_event(EventSwitch{ config->name.c_str(), time, new_state });
    }
}

bool Switch::is_on() const
{
    return state;
}

bool Switch::is_on_high() const
{
    return is_high;
}

void Switch::switch_on()
{
    if (state == SWITCH_OFF) {
        log_message(TRACE, "switching ON switch " + config->name);

        write_on();
        state = SWITCH_ON;
    }
}

void Switch::switch_off()
{
    if (state == SWITCH_ON) {
        log_message(TRACE, "switching OFF switch " + config->name);

        write_off();
        state = SWITCH_OFF;
    }
}

void Switch::write_on()
{
    log_message(TRACE, "switching switch " + config->name + " to HIGH");

    digitalWrite(config->gpio, HIGH);
    is_high = true;
}

void Switch::write_off()
{
    log_message(TRACE, "switching switch " + config->name + " to LOW");

    digitalWrite(config->gpio, LOW);
    is_high = false;
}
}
