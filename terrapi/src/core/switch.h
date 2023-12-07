#pragma once

#include "expr.h"

namespace terra
{
struct SwitchConfig;

enum SwitchState
{
    SWITCH_OFF = 0,
    SWITCH_ON  = 1,
};

class Switch final
{
public:
    /// @pre \p gpio is empty
    Switch(SwitchConfig* config, const Expr& expr);

    /// called each tick
    void update(uint64_t time);

    ///
    bool is_on() const;

    bool is_on_high() const;

    void switch_on();
    void switch_off();

private:
    /// Write the GPIO pin to high.
    void write_on();

    /// Write the GPIO pin to low.
    void write_off();

    SwitchConfig* config;

    Expr rule;

    SwitchState state = SWITCH_OFF;

    bool oscillate   = false;
    bool is_high     = false;
    uint64_t next_toggle = -1;
};
}
