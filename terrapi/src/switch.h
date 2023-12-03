#pragma once

#include "expr.h"

namespace terra
{
struct SwitchConfig;

class Switch final
{
public:
    /// @pre \p gpio is empty
    Switch(SwitchConfig* config, const Expr& expr);

    /// called each tick
    void update(Time time);

    ///
    bool is_on() const;

    bool is_on_high() const;

    void switch_on();
    void switch_off();

private:
    void write_on();
    void write_off();

    SwitchConfig* config;

    Expr rule;

    /// ON/OFF
    bool state = false;

    bool oscillate   = false;
    bool is_high     = false;
    Time next_toggle = -1;
};
}
