// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

#include "switch.h"

#include "wiringPi.h"

#include "config.h"
#include "logger.h"
#include "core/core.h"

namespace terra
{
Switch::Switch(DeviceConfig* config, const Expr& expr)
    : config(config), rule(expr)
{
    pinMode(config->gpio, OUTPUT);
    write_off();

    if (config->oscillation_high_ms == -1 || config->oscillation_low_ms == -1) {
        oscillate = false;
    } else {
        oscillate = true;
    }
}

void Switch::update(uint64_t now_ms)
{
    const auto previous_state = state;
    // Test if the switch should be on or off.
    const auto new_state = SwitchState(rule->evaluate());

    if (new_state) {
        // first switch
        if (state == SWITCH_OFF && oscillate) {
            next_toggle_ms = now_ms + config->oscillation_low_ms;
        }

        switch_on();
    } else {
        switch_off();
    }

    if (state == SWITCH_ON && oscillate) {
        // Is already on, check if it's time to toggle.
        if (now_ms >= next_toggle_ms) {
            // toggle
            if (is_high) {
                write_off();
                next_toggle_ms = next_toggle_ms + config->oscillation_low_ms;
            } else {
                write_on();
                next_toggle_ms = next_toggle_ms + config->oscillation_high_ms;
            }
        }
    }

    if (previous_state != new_state) {
        core().record_event(Event{
            config->name.c_str(),
            now_ms,
            new_state
        });
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
        LOG(INFO, "switching '{}' ON", config->name);

        write_on();
        state = SWITCH_ON;
    }
}

void Switch::switch_off()
{
    if (state == SWITCH_ON) {
        LOG(INFO, "switching '{}' OFF", config->name);

        write_off();
        state = SWITCH_OFF;
    }
}

void Switch::write_on()
{
    LOG(TRACE, "switching '{}' to HIGH", config->name);

    digitalWrite(config->gpio, HIGH);
    is_high = true;
}

void Switch::write_off()
{
    LOG(TRACE, "switching '{}' to LOW", config->name);

    digitalWrite(config->gpio, LOW);
    is_high = false;
}
}
