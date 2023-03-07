#include "switch.h"

#include "wiringPi.h"

#include "config.h"
#include "logger.h"

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

void Switch::update(Time time)
{
    const auto new_state = rule->evaluate();

    if (new_state) {
        // first switch
        switch_on();
    } else {
        switch_off();
    }

    if (state && oscillate) {
        if (time > next_toggle) {
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
}

bool Switch::is_on() const
{
    return state;
}

void Switch::switch_on()
{
    if (state == false) {
        write_on();
        state = true;
    }
}

void Switch::switch_off()
{
    if (state == true) {
        write_off();
        state = false;
    }
}

void Switch::write_on()
{
    log_message("trace", "switching ON switch " + config->name);

    digitalWrite(config->gpio, HIGH);
    is_high = true;
}

void Switch::write_off()
{
    log_message("trace", "switching OFF switch " + config->name);

    digitalWrite(config->gpio, LOW);
    is_high = false;
}
}
